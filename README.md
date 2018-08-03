# [Tutorial - Build a DIY security camera with neural compute stick](https://www.dlology.com/blog/build-a-diy-security-camera-with-neural-compute-stick-part-2/) 
Arduino sketch to add an turret and turn the camera which follows people around.

## CamGimbal
Arduino pro micro board / other atmega32u4 MCU based Arduino compatible board.

The servo can move in either of the two modes.
* An absolute angle in degree.
* A relative angle + or - in degree.

### Get started
Wire servo signal to Pin 4.
Compile and download the code, then connect to Arduino serial port with 9600 baud rate, send HEX `(0xB4 0xFE)` i.e. `(180, 254)` in decimal will turn servo to `180` degree.
Send HEX `(0xB9 0xFF)` i.e. `(180+5, 255)` in decimal will turn servo `5` degrees relative to current servo angle.

### Optional IDE
[Arduino IDE for Visual Studio](https://www.visualmicro.com/) plugin.