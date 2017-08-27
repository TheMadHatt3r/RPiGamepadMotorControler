# XboxMotorControler
Semi Reliable Control via Python/BASH.<br>
OR<br>
Real time Control via C++.

## About
This software is designed to read in a generic game pad (Xbox controller etc.) and transmit its 
analog stick values via serial to a Sabertooth 2x12 motor controller.
Also comes with /etc/rc.local example modification to auto start the software at startup so you can run headless.

## Requirements
For the Python/BASH software:
1. Written and tested in Python 3.5, requires "inputs".

For the C++ software:
1. No real requirements, it is compiled for Raspberrian 16-AUG-2017 edition.

## Credits
1. Made use of Drew Noakes Light Weight Joystick C++ Class https://github.com/drewnoakes/joystick.
2. Early Python version used Inputs by Zeth https://github.com/zeth/inputs.
3. The rest of the code is mine to the best of my knowledge.
