#!/bin/bash
#######################################################
# Reads in file and sends data out 
# as raw bytes to serial interface.
#
# Collin Matthews
# 20-AUG-2017
#######################################################

SERIAL='/dev/serial0'
BAUD=9600
FILE_L='/home/pi/dataL.dat'
FILE_R='/home/pi/dataR.dat'

#Setup GPIO
echo "4" > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio4/direction


#stty -F $SERIAL $BAUD raw -echo -echoe -echok -echoctl -echoke
stty -F $SERIAL $BAUD raw
while true
do
  WAIT=1
  if [ -e  $FILE_L ]; then
    echo "1" > /sys/class/gpio/gpio4/value
    Q=`cat $FILE_L`
    printf $Q > $SERIAL
    #hexdump -v -e '7/4 "%10d "' -e '"\n"' $FILE_L
    rm $FILE_L
    echo "0" > /sys/class/gpio/gpio4/value
    WAIT=0
  fi

  if [ -e  $FILE_R ]; then
    echo "1" > /sys/class/gpio/gpio4/value
    Q=`cat $FILE_R`
    printf $Q > $SERIAL
    #hexdump -v -e '7/4 "%10d "' -e '"\n"' $FILE_R
    rm $FILE_R
    echo "0" > /sys/class/gpio/gpio4/value
    WAIT=0
  fi

  if [ $WAIT == 1 ]; then
    sleep 0.005
  fi

done