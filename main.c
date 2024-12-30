	#include <stdio.h>
	#include <LPC23xx.H>                    /* LPC23xx definitions                */
	#include "LCD.h"                        /* Graphic LCD function prototypes    */

	extern int j,hr,min,sec;
	extern char hrVal[2];
	extern char minVal[2];
	extern char secV[2];
	
	/* Import external IRQ handlers from IRQ.c file                               */
	extern __irq void T0_IRQHandler  (void);
	extern void PWM_SetDutyCycle();
	extern void PWM_Init();
	extern void updateRTCTimer();
	extern void RTCTimerEnable();
	extern void setupTimer();
	extern void sendPulse();
	extern void delayMS();
	extern float calDistance();

	/* Import external variables from IRQ.c file                                  */
	extern unsigned char clock_1s;

	int main(void) 
	{
			char cVal[20];
			int n=2, m=3;
			float distance=0.0;
			int release_time_min = 0;		//mins
			int release_time_max = 30 ;  //secs
			int petPresence = 0, fed = 0, capTime=0;
	
		int feeding_time_flag = 0, pet_presence_flag = 0, isClosed = 1;
		setupTimer(n, m);
	
		RTCTimerEnable();
		PWM_Init();
		lcd_init();
		lcd_clear();
		//lcd_print ("Local Time");
		set_cursor (0, 1);
		lcd_print ("00:00:00");
		PWM_SetDutyCycle(5);
		
		while (1) {                           /* Loop forever                       */
			updateRTCTimer();
			sendPulse(n);
			distance = calDistance(m);
			
			if(distance < 30) {
				set_cursor(0,0);
				sprintf(cVal, "Dis:%.2f %d", distance, release_time_min);
				set_cursor(0,0);
				lcd_print(cVal);
				petPresence = 1;
			} else if (distance < 1 ) {
				set_cursor(0,0);
				lcd_print("Dis too less");
			}else {
				petPresence = 0;
				set_cursor(0,0);
				sprintf(cVal, "Dis too far");
				lcd_print(cVal);
			}
			
			if(min == release_time_min && sec < release_time_max)
			{
				set_cursor(0, 10);
				lcd_print("");
				if(petPresence && !capTime) {
						capTime = sec;
						PWM_SetDutyCycle(10); //7
						isClosed = 0;
						fed = 1;
						
				}
				set_cursor(12, 0);
				sprintf(cVal, "%d", capTime);
				lcd_print(cVal);
				
			}
			if(sec ==(capTime + 2) && !isClosed) {
							PWM_SetDutyCycle(5);
							isClosed = 1;
				}	
				if(sec == release_time_max) {
					release_time_min = (release_time_min + 1) % 60 ;
					fed = 0; 
					if(isClosed) capTime = 0;		
			}

			delayMS(1000); //wait 1 second for next update

			lcd_clear();
		}
}
