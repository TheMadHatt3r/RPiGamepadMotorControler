#!/bin/bash
######################################
# Reads in file and sends data out 
# as raw bytes to serial interface.
#
#

SERIAL='/dev/serial0'
BAUD=9600
FILE='./data.dat'


stty -F $SERIAL $BAUD raw -echo -echoe -echok -echoctl -echoke
while true
do
  if [ -e  $FILE ]; then
    Q=`cat $FILE`
    printf $Q > $SERIAL
    rm $FILE
  else
    sleep 0.005
  fi
  
done