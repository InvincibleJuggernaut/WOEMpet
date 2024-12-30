#include <LPC23xx.h>
#include <stdio.h> 
#include <stdlib.h> 
#include "LCD.h"

int i, dutyCycle;

void PWM_Init() 											// Set pin function for P1.18 as PWM1
{    	
	
    PINSEL3 |= (2 << 4); 							// Set bits 4-5 to 10 (AF2) for PWM1.1

    // Set PWM mode for PWM1
    PWM1PCR = (1 << 9); 							// Enable PWM1 output (PWM1.1). Set bit 9 = 1
    PWM1MCR = (1 << 1); 							// Reset counter on matching value in PWM1MR0
    
    // Set PWM frequency
    PWM1MR0 = 240000; 								// Set period to 20 ms (assuming 1 MHz clock)
    PWM1TCR = (1 << 0) | (1 << 3); 		// Enable PWM and counter. 0th bit to enable counter and 3rd bit to enable PWM

}

void WaitTime(int wait_time)
{
		for (i = 0; i < wait_time; i++);				// Approx 4-5s
}
void PWM_SetDutyCycle(int dutyCycle) 				// MG995 duty cycle mapping: 1ms = 0° (5%), 1.5ms = 90° (7.5%), 2ms = 180° (10%)
{
    PWM1MR1 = (dutyCycle * PWM1MR0) / 100; 	// Set duty cycle to value set as 1 time period
    PWM1LER = (1 << 1); 										// Enable PWM1.1 latch, ensures new value for PWM1MR1 is applied at next update of PWM signal
}

 int trial_run_servo() 
 {
	 lcd_init();
	 lcd_clear();
     PWM_Init();
			while (1) 
			{
				PWM_SetDutyCycle(7.5);
			}
	}
