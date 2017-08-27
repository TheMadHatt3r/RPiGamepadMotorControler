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


int openSerialPort(void);
void closeSerialPort(int sfd);
void setupSerialPort(int sfd);



int main(int argc, char** argv)
{
    // Create an instance of Joystick
    Joystick joystick("/dev/input/js0");

    // Ensure that it was found and that we can use it
    if (!joystick.isFound())
    {
        printf("Failed to open /dev/input/js0.\n");
        exit(1);
    }

    //Startup serial interface
    int sfd = openSerialPort();
    if (sfd == -1)
        exit(1);
    setupSerialPort(sfd);
    //Close since I'm just testing.
    closeSerialPort(sfd);
    

    while (true)
    {
    // Restrict rate
    usleep(1000);

    // Attempt to sample an event from the joystick
    JoystickEvent event;
    if (joystick.sample(&event))
    {
        if (event.isButton())
        {
            printf("Button %u is %s\n",
            event.number,
            event.value == 0 ? "up" : "down");
        }
        else if (event.isAxis())
        {
            printf("Axis %u is at position %d\n", event.number, event.value);
        }
    }

    }
    }

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

void setupSerialPort(int sfd)
{
    struct termios options;
    tcgetattr(sfd, &options);
    cfsetspeed(&options, B9600);
    options.c_cflag &= ~CSTOPB;
    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;
    cfmakeraw(&options);
    tcsetattr(sfd, TCSANOW, &options);
}

void closeSerialPort(int sfd)
{
    if (sfd != -1)
        close(sfd);  
}
