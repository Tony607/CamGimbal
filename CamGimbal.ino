// Tutorial - Build a DIY security camera with neural compute stick : 
// https://www.dlology.com/blog/build-a-diy-security-camera-with-neural-compute-stick-part-2/
// Note that you can change MIN_PULSE_WIDTH and MAX_PULSE_WIDTH definition in Servo.h to meet the your Servo
// so 0-180 Degree is the max and min angle it can turn.
// Connect to 9600 baudrate serial port, send HEX (0xB4 0xFE) i.e. (180, 254) in decimal will turn servo to 180 degree.
// Send HEX (0xB9 0xFF) i.e. (180+5, 255) in decimal will turn servo 5 degrees relative to current servo angle.
#include <Servo.h>
// Serialport baudrate.
#define BAUD 9600
// The number of servos to use.
// In our case we only have X axis servo.
#define SERVO_COUNT 1
// X axis servo pin.
#define servo_x 4
// Servo can move in either to a absolute angle or a relative angle + or - in degree.
#define CMD_ABSOLUTE 0XFE
#define CMD_DELTA 0XFF
// Servo objects.
Servo servos[SERVO_COUNT];
// Serial received buffer.
unsigned char buf[SERVO_COUNT + 1];
// The target angle for each servo in degree.
unsigned char servoAngleTarget[SERVO_COUNT];
// The actual tuning angle of each servo in degree.
float servoAngleNow[SERVO_COUNT];
// The Arduino Pin of each servo.
uint8_t servoPins[SERVO_COUNT] = { servo_x };

// Number of servos attached.
uint8_t servoAttached = 0;
// The max tuning speed in degree per interval.
unsigned char moveSpeed = 1;
// The servo tunning interval.
const unsigned long interval = 20; //ms
// Last timestamp server angle changed.
unsigned long last_sent;
void setup()
{
	Serial.begin(BAUD);
	// Initialize serial receive buffer and variables.
	initVars();
}

void loop()
{
	int j = 0;
	if (Serial.available() > 0) {
		// get incoming byte:
		int inByte = Serial.read();
		parseSerial(inByte);
	}
	//Update servo angle every 20ms(50hz).
	unsigned long now = millis();
	if (now - last_sent >= interval)
	{
		last_sent = now;
		{
			for (j = 0; j < SERVO_COUNT; j++) {
				if (servoAngleTarget[j] != servoAngleNow[j]) {
					if (servoAttached == 0 && !servos[j].attached()) {
						servos[j].attach(servoPins[j]);
						servoAttached++;
					}
				}
				else {
					if (servos[j].attached()) {
						servos[j].detach();
						printAngleNow(j);
						if (servoAttached > 0) {
							servoAttached--;
						}
					}
				}
				if (servos[j].attached()) {
					servoAngleNow[j] = getToAngle(servoAngleNow[j], servoAngleTarget[j]);
					servos[j].write(servoAngleNow[j]);
				}
			}
		}
	}
}

// Parses the serial received data.
// args : 
// b : The byte received.
void parseSerial(int b)
{
	static unsigned char bufindex = 0;
	char x;
	if (bufindex > SERVO_COUNT) { // Buffer overflow, reset the buffer index.
		bufindex = 0;
	}
	buf[bufindex] = b;
	if ((buf[bufindex] == CMD_ABSOLUTE) && (bufindex >= SERVO_COUNT))//End of command to absolute angle in degree.
	{
		for (x = 0; x < SERVO_COUNT; x++)
		{
			servoAngleTarget[SERVO_COUNT - 1 - x] = buf[bufindex - x - 1];
			printAngleNow(SERVO_COUNT - 1 - x);
		}
		bufindex = 0;
	}
	else if ((buf[bufindex] == CMD_DELTA) && (bufindex >= SERVO_COUNT))//End of command to relative angle in degree(delta).
	{
		for (x = 0; x < SERVO_COUNT; x++)
		{
			servoAngleTarget[SERVO_COUNT - 1 - x] = clampAngle(servoAngleNow[SERVO_COUNT - 1 - x] + buf[bufindex - x - 1] - 90);
			printAngleNow(SERVO_COUNT - 1 - x);
		}
		bufindex = 0;
	}
	else {
		bufindex++;
	}
}
// Send the current servo angle through serialport given servo index, starting from 0.
void printAngleNow(int servoIndex) {
	Serial.print(servoIndex);
	Serial.print('-');
	Serial.println(int(servoAngleNow[servoIndex]));
}
// Helper function to clamp angle between 0~180 degree.
unsigned char clampAngle(int angle) {
	if (angle < 0) {
		angle = 0;
	}
	else if (angle > 180) {
		angle = 180;
	}
	return (unsigned char)angle;
}
// Initialize serial receiver buffer and variables.
void initVars()
{
	int i = 0;
	for (i = 0; i < SERVO_COUNT + 1; i++)
	{
		buf[i] = 90;
	}
	for (i = 0; i < SERVO_COUNT; i++)
	{
		servoAngleTarget[i] = 90;
		servoAngleNow[i] = 90;
	}
	for (i = 0; i < SERVO_COUNT; i++) {
		servoAngleNow[i] = 90;
	}
	servoAngleTarget[0] = 80;
}
// Lerp servo from current(now) angle to target angle with speed.
// Returns the next lerp angle in degree.
float getToAngle(float now, int target) {
	if (abs(target - now) <= moveSpeed) {
		return target;
	}
	return now + moveSpeed*(target - now) / abs(target - now);
}
