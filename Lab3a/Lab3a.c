//3a

#include <msp430.h> 
/**
 * main.c
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR |= 0x40;              //set Pin 1.6 Red LED as output
	P1REN |= 0x08;              //enabling the resistor at Pin 1.3
	P1OUT &= 0x40;              //turning off Pin 1.6
	P1OUT |= 0x08;

	while(1)
	{
	  if(P1IN & P1REN)
	      P1OUT &= ~0x40;       // Red LED turns off
	  else
	      P1OUT |= 0x40;        // Red LED turns on
	}

}
