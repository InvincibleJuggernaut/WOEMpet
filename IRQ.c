#include <LPC23xx.H>                    /* LPC23xx definitions                */

extern unsigned char clock_1s;                 /* Flag activated each second         */

//int x;
//int flag = 0;
/* Timer0 IRQ: Executed periodically                                          */
__irq void T0_IRQHandler (void) {
  static int clk_cntr;
  

  clk_cntr++;
  if (clk_cntr >= 1000) {
    clk_cntr = 0;
    clock_1s = 1;                       /* Activate flag every 1 second       */
  }

 T0IR        = 1;                      /* Clear interrupt flag               */
  VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}


