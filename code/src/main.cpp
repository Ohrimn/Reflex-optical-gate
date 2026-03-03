#include <cstdint>
#include <Arduino.h>
#include <driver/ledc.h>

uint16_t fPWM = 5000;
uint8_t dutyCycle = 1;

volatile bool newPulse = false;
uint32_t pulseTime = 0;
volatile uint32_t pulseCount = 0;
bool block = false;
uint32_t starttime = 0;
uint32_t pulperioda = 0;
uint32_t perioda = 0;
uint32_t firstPulperioda = 0;
bool prvniperioda = false;

void IRAM_ATTR handlePulse()
{
	newPulse = true;
	pulseCount++;
}

void setup()
{
	// PWM setup
	ledcSetup(0, fPWM, 8);	 // kanál 0, fPWM = 5000 Hz, rozlišení 8 bitů
	ledcAttachPin(10, 0);	 // pin 6 na kanál 0
	ledcWrite(0, dutyCycle); // duty cycle 1/255

	// Serial setup
	Serial.begin(115200);

	// pin 0 jako vstup
	pinMode(5, INPUT);

	// interript na pinu 0
	attachInterrupt(digitalPinToInterrupt(5), handlePulse, RISING);
}

void loop()
{
	if (newPulse == true && pulseCount >= 3 && block == false)
	{
		pulperioda = millis() - starttime;

		if (!prvniperioda)
		{
			firstPulperioda = pulperioda;
			prvniperioda = true;
		}
		else
		{
			perioda = firstPulperioda + pulperioda;
			Serial.println(perioda);

			firstPulperioda = 0;
			prvniperioda = false;
		}

		pulperioda = 0;
		starttime = 0;

		block = true;
		starttime = millis();
	}
	else if (block == true && newPulse == false)
	{
		block = false;
	}

	pulseCount = 0;
	newPulse = false;
	delayMicroseconds(600);
}