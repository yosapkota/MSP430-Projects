//3b

#include <msp430.h> 
/**
 * main.c
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR |= 0x40;              //set Pin 1.6 Red lED as output
	P1REN |= 0x08;
	P1OUT &= 0x40;
	P1OUT |= 0x08;

	while(1)
	{
	 if((P1IN & P1REN) == 0){
	      P1OUT ^= 0x40;
	      __delay_cycles(1000000/20);
	 }

	 else{
	      P1OUT ^= 0x40;
	      __delay_cycles(1000000/2);
	 }
	    }

}
