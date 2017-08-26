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


#stty -F $SERIAL $BAUD raw -echo -echoe -echok -echoctl -echoke
stty -F $SERIAL $BAUD raw
while true
do
  WAIT=1
  if [ -e  $FILE_L ]; then
    Q=`cat $FILE_L`
    printf $Q > $SERIAL
    #hexdump -v -e '7/4 "%10d "' -e '"\n"' $FILE_L
    rm $FILE_L
    WAIT=0
  fi

  if [ -e  $FILE_R ]; then
    Q=`cat $FILE_R`
    printf $Q > $SERIAL
    #hexdump -v -e '7/4 "%10d "' -e '"\n"' $FILE_R
    rm $FILE_R
    WAIT=0
  fi

  if [ $WAIT == 1 ]; then
    sleep 0.005
  fi

done