#include <msp430.h> 
/**
 * main.c
 */
void main(void)
{

   WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1DIR |= 0x40;
	P1OUT &= ~0x40;
	P1SEL |= 0X40;

	P1REN |= 0x08;
	P1OUT |= 0x08;

	TA0CCR0 = 1000;
	TA0CCTL1 = OUTMOD_7;
	TA0CCR1 = 0;
	TA0CTL = TASSEL_2 + MC_1;

	while(1){
	    if((P1IN & 0x08) != 0x08)
	        __delay_cycles(50000);

	    TA0CCR1 += 100;

	    if(TA0CCR1 > 1000)
	        TA0CCR1 = 0;
	}
}
