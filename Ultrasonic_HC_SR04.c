#include <LPC23XX.H>
#include <stdio.h>
#include "LCD.h"

	int echoTime=0;
	float distance=0;
	char cVal[10];
	int k;
	int pulsecnt;
	
void delayUS(unsigned long microseconds) //Using Timer0
{
	T1TCR = 0x02; //Reset Timer
	T1TCR = 0x01; //Enable timer
	while(T1TC < microseconds); //wait until timer counter reaches the desired delay
	T1TCR = 0x00; //Disable timer
}
void initTimer0(void)
{
	PCONP |= (1<< 2);
	//Assuming PCLK = 60Mhz
	T1CTCR = 0x0;
	T1PR = 11; //60 clock cycles @60Mhz = 1 us
	T1TCR = 0x02; //Reset Timer
}

void startTimer0(void)
{
	T1TCR = 0x02; //Reset Timer
	T1TCR = 0x01; //Enable timer
}

unsigned int stopTimer0(void)
{
	T1TCR = 0x00; //Disable timer
	return T1TC;
}

void delayMS(unsigned int milliseconds)
{
	delayUS(milliseconds * 1000);
}
void setPins(int n, int m) {
	#define ECHO (1<<m)
	#define TRIG (1<<n)
	IODIR0 |= TRIG;    //Set P0.2(TRIG) as output
	IODIR0 &= ~(ECHO); //Set P0.3(ECHO) as input (explicitly)
	IOCLR0 |= TRIG;
}
void setupTimer(int n, int m){
	lcd_init();
	lcd_clear();

	initTimer0(); //Init Timer for delay functions
	setPins(n, m);

}
 /* read the timer and calculate from timer the distance */
float calDistance(int m){
	#define ECHO (1<<m)
	
	while(!(IOPIN0 & ECHO)); //Wait for a HIGH on ECHO pin
		startTimer0(); //Start counting
		while(IOPIN0 & ECHO); //Wait for a LOW on ECHO pin
		echoTime = stopTimer0(); //Stop counting and save value(us) in echoTime
		distance =0;
		distance = (0.0343 * echoTime)/2; //Find the distance
		return distance;
}

void sendPulse(int n)
{
		#define TRIG (1<<n)
	
		IOSET0 |= TRIG;
		delayUS(10);
		IOCLR0 |= TRIG;
}
int trial_run_ultrasonic(void)
{
	//Set P0.2 LOW initially
	int n=2, m=3;
	float distance=0.0;
	setupTimer(n, m);

	while(1)
	{
		lcd_clear();
		sendPulse(n);
		distance = calDistance(m);
		if(distance < 40) {
			set_cursor(0,0);
			sprintf(cVal, "Dis: %f", distance);
			set_cursor(0,0);
			lcd_print(cVal);
		} else if (distance < 1 ) {
			set_cursor(0,0);
			lcd_print("Dis too less");
		}else {
			set_cursor(0,0);
			sprintf(cVal, "Dis too far");
			lcd_print(cVal);
		}
		
		set_cursor(0,1);
		lcd_print("Works");
		delayMS(1000); //wait 1 second for next update
	}

}