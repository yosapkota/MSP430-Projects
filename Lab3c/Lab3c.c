//3c

#include <msp430.h> 
/**
 * main.c
 */
void main(void)
{
	    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	    P1DIR |= 0x41;              //set Pin 1.6 Red LED and 1.0 Green LED as output
	    P1REN |= 0x08;
	    P1OUT |= 0x08;

	    while(1)
	    {
	     if((P1IN & P1REN)){
	          P1OUT ^= 0x40;
	          P1OUT &= ~0x01;
	          __delay_cycles(500000);
	     }
	     else{
	          P1OUT ^= 0x01;
	          P1OUT &= ~0x40;
	          __delay_cycles(500000);
	          }
	       }

}
