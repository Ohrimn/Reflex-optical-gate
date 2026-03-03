#include <cstdint>
#include <Arduino.h>
#include <driver/ledc.h>

uint16_t fPWM = 40000;
uint8_t dutyCycle = 3;

volatile bool newPulse = false;
uint32_t pulseTime = 0;
volatile uint32_t pulseCount = 0;
bool block = false;
uint32_t starttime = 0;
uint32_t pulperioda = 0;
uint32_t perioda = 0;
uint32_t firstPulperioda = 0;
bool prvniperioda = false;

uint8_t pulse_count = 8;
uint32_t delay_time = 0;

void IRAM_ATTR handlePulse()
{
	newPulse = true;
	pulseCount++;
}

void setup()
{
	// PWM setup
	ledcSetup(0, fPWM, 8);	 // kanál 0, fPWM, rozlišení 8 bitů
	ledcAttachPin(10, 0);	 // pin 6 na kanál 0
	ledcWrite(0, dutyCycle); // duty cycle 1/255

	// Serial setup
	Serial.begin(460800);

	// pin 0 jako vstup
	pinMode(5, INPUT);

	// interript na pinu 0
	attachInterrupt(digitalPinToInterrupt(5), handlePulse, RISING);

	delay_time = 1000000 / fPWM * pulse_count + 5; // čas mezi pulzy v mikrosekundách

}

void loop()
{
	if (newPulse == true && pulseCount >= pulse_count && block == false)
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
			firstPulperioda = 0;
			prvniperioda = false;

			Serial.println(perioda);
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
	delayMicroseconds(delay_time);
} 