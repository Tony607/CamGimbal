// Note that we changed Servo.h to meet the micro Servo so 0-180 Degree is the max and min angle it can turn
//connect to 115200 baudrate serial port, send HEX 0xB4 will turn servo to 180 degree,etc.
// MIN_PULSE_WIDTH -->500     // the shortest pulse sent to a servo  
// MAX_PULSE_WIDTH -->2146     // the longest pulse sent to a servo 
#include <Servo.h>

#define BAUD 9600
#define SERVO_COUNT 1

#define servo_x 4
//buffer size for the audio library
#define buffSize 128

Servo servos[SERVO_COUNT];
unsigned char buf[SERVO_COUNT * 3];
unsigned char servoAngleTarget[SERVO_COUNT];
float servoAngleNow[SERVO_COUNT];
uint8_t servoPins[SERVO_COUNT] = { servo_x };

/// <summary>
/// Number of servos attached
/// </summary>
uint8_t servoAttached = 0;
/// <summary>
/// The inversed lerp rate for the servo
/// </summary>
unsigned char lerpRate_inv = 20;//1/20 = 0.05
unsigned char moveSpeed = 1;
const unsigned long interval = 20; //ms
								   // When did we last send?
unsigned long last_sent;
void setup()
{
	Serial.begin(9600);
	initbuf();
	//init servos
	int j;
	for (j = 0; j<SERVO_COUNT; j++) {
		servoAngleNow[j] = 90;
	}
	servoAngleTarget[0] = 80;

}

void loop()
{
	int j = 0;
	if (Serial.available() > 0) {
		// get incoming byte:
		int inByte = Serial.read();
		parseSerial(inByte);
	}
	//update servo angle every 20ms(50hz)
	unsigned long now = millis();
	if (now - last_sent >= interval)
	{		
		last_sent = now;
		{
			for (j = 0; j<SERVO_COUNT; j++) {
				if (servoAngleTarget[j] != servoAngleNow[j]) {
					if (servoAttached == 0 && !servos[j].attached()) {
						servos[j].attach(servoPins[j]);
						servoAttached++;
					}
				}
				else {
					if (servos[j].attached()) {
						servos[j].detach();
						if (servoAttached>0) {
							servoAttached--;
						}

					}
				}
				if (servos[j].attached()) {
					servoAngleNow[j] = getToAngle(servoAngleNow[j], servoAngleTarget[j], lerpRate_inv);
					servos[j].write(servoAngleNow[j]);
				}
			}
		}
	}
}

/// <summary>
/// Parses the serial received data.
/// </summary>
/// <param name="b">The byte received.</param>
void parseSerial(int b)
{
	static unsigned char bufindex = 0;
	char x;
	if (bufindex>SERVO_COUNT * 3 - 1) {
		bufindex = 0;
	}
	buf[bufindex] = b;
	if ((buf[bufindex]>180) && (bufindex >= SERVO_COUNT))//stop sign
	{
		for (x = 0; x<SERVO_COUNT; x++)
		{
			servoAngleTarget[SERVO_COUNT - 1 - x] = buf[bufindex - x - 1];
		}
		bufindex = 0;
		Serial.print('#');
		//UDR0 = 0xAF;
	}
	else {
		bufindex++;
	}
}
/// <summary>
/// Initalize serial receiver buffer.
/// </summary>
void initbuf()
{
	int i = 0;
	for (i = 0; i<SERVO_COUNT * 3; i++)
	{
		buf[i] = 90;
	}
	for (i = 0; i<SERVO_COUNT; i++)
	{
		servoAngleTarget[i] = 90;
		servoAngleNow[i] = 90;
	}
}


/// <summary>
/// Linear interpolation the angle from now to target.
/// </summary>
/// <param name="now">current angle.</param>
/// <param name="target">target angle.</param>
/// <param name="rate_inv">The lerp rate inversed.</param>
/// <returns></returns>
float lerpAngle(float now, int target, int rate_inv) {
	if (abs(target - now) < 1) {
		return target;
	}
	return now + (target - now) / rate_inv;
}

float getToAngle(float now, int target, int rate_inv) {
	if (abs(target - now) <= moveSpeed) {
		return target;
	}
	return now + moveSpeed*(target - now) / abs(target - now);
}
