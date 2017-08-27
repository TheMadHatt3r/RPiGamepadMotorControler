/**************************************************
* Basic C++ Joystick Motor Controler Over Serial.
*   Uses Drew Noakes Joystick C++ Class.
*   Interfaces to Sabertooth MCU over Serial.
*
*
* Collin Matthews - 27AUG2017
***************************************************/

#include "joystick.hh"
#include <unistd.h>
//For Serial
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
//Catch SIGNALS
#include <signal.h>


#define STICK_ZERO_THRESH 800
#define STICK_ONE_THRESH  31000
#define LEFT 1
#define RIGHT 4

int openSerialPort(void);
void closeSerialPort(int sfd);
void setupSerialPort(int sfd);
int filterScaleInput(JoystickEvent event);
void setMotorSpeed(int side, char speed, int sfd);
void ctrlCExit(int sig);

volatile sig_atomic_t ctrlc = 0;



int main(int argc, char** argv)
{
    printf("Starting program. No printout due to headless design... Unless errors happen.\n");
    
    //Register Signals
    signal(SIGINT, ctrlCExit); 
    
    //Create an instance of Joystick
    Joystick joystick("/dev/input/js0");

    //Ensure that it was found and that we can use it
    if (!joystick.isFound())
    {
        printf("Failed to open /dev/input/js0.\n");
        exit(1);
    }

    //Startup serial interface
    int sfd = openSerialPort();
    if (sfd == -1)
    {
        printf("Failed to open /dev/serial0.\n");
        exit(1);
    }
    setupSerialPort(sfd);
    //closeSerialPort(sfd);
    

    while (true)
    {
        // Restrict rate
        usleep(1000);

        // Attempt to sample an event from the joystick
        JoystickEvent event;
        if (joystick.sample(&event))
        {
            if (event.isAxis())
            {
                //printf("Axis %u is at position %d\n", event.number, event.value);
                int val = filterScaleInput(event);
                if (val != -100)
                {
                    //printf("Axis %u is at position %d\n", event.number, val);
                    if (event.number == LEFT)
                        setMotorSpeed(LEFT, val, sfd);
                    else if (event.number == RIGHT)
                        setMotorSpeed(RIGHT, val, sfd);
                    
                }
            }
        }
        //Check sigterm
        if (ctrlc == 1)
        {
            closeSerialPort(sfd);
            printf("\nSafe Exit Triggered via SIGTERM.\n");
            exit(0);
        }
    }
}


/**********************************************
* Function handles SIGTERM for safe closure.
***********************************************/
void ctrlCExit(int sig){ // can be called asynchronously
  ctrlc = 1; // set flag
}

/**********************************************
* Function to set proper offsets for MCU.
* Sends to serial interface.
* Accepts -64 to 64 char. and LEFT/RIGHT for side.
* Returns -100 on other axis.
*
* Motor 1: 1=FullRev 64=Stop 127=FullFwd
* Motor 2: 128=FullRev 192=Stop 255=FullFwd
* Sending 0 will stop both motors.
*
***********************************************/
void setMotorSpeed(int side, char speed, int sfd)
{    
    unsigned char buf[2];
    buf[0] = 0;
    
    if(side==LEFT)
    {
        speed = speed + 64;
        if (speed == 0)
            speed = 1; //Left has 1 less precision.
        //printf("Left:%d\n",speed);
        buf[0] = speed;
        write(sfd, &buf, 1);
    }
    else
    {
        speed = speed + 192;
        //printf("RIGHT:%d\n",speed);
        buf[0] = speed;
        write(sfd, &buf, 1);
    }
    
        
}

/**********************************************
* Function to scale and filter joystick input.
* Returns -64 to 64 int.
* Returns -100 on other axis.
***********************************************/
int filterScaleInput(JoystickEvent event)
{
    if ((event.number == 1) || (event.number == 4))
    {
        int val = event.value;
        if ((event.value > -1*STICK_ZERO_THRESH) && (event.value < STICK_ZERO_THRESH))
            val = 0;
        else if (event.value > STICK_ONE_THRESH)
            val = 32767;
        else if (event.value < -1*STICK_ONE_THRESH)
            val = -32768;
        val = val/512;
        return val;
    }
    return -100;
}

/**********************************************
* Open serial port and returns pointer to it.
***********************************************/
int openSerialPort()
{
    int sfd = open("/dev/serial0", O_RDWR | O_NOCTTY ); 
    if (sfd == -1){
        printf ("Error no is : %d\n", errno);
        printf("Error description is : %s\n",strerror(errno));
        return(-1);
    }
    return sfd;
}

/**********************************************
* Accepts serial port pointer and configures it.
***********************************************/
void setupSerialPort(int sfd)
{
    struct termios options;  // Terminal options
    
    fcntl(sfd, F_SETFL);                 // Configure port reading
    tcgetattr(sfd, &options);            // Get the current options for the port
    cfsetispeed(&options, B9600);     // Set the baud rates to 230400
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD);    // Enable the receiver and set local mode
    options.c_cflag &= ~PARENB;             // No parity bit
    options.c_cflag &= ~CSTOPB;             // 1 stop bit
    options.c_cflag &= ~CSIZE;              // Mask data size
    options.c_cflag |=  CS8;                // Select 8 data bits
    options.c_cflag &= ~CRTSCTS;            // Disable hardware flow control  

    // Enable data to be processed as raw input
    options.c_lflag &= ~(ICANON | ECHO | ISIG);

    // Set the new attributes
    tcsetattr(sfd, TCSANOW, &options);
}

/**********************************************
* Closes serial port given pointer to it.
***********************************************/
void closeSerialPort(int sfd)
{
    if (sfd != -1)
        close(sfd);  
}
