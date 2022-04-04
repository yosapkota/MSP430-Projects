#include <msp430.h> 

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1DIR |= 0x40;   // config P1.6 (RED LED) to an output
	P1OUT &= ~0x40;  // initially P1.6 turned off

	P1REN |= 0x08;   //Enable resistor to push button
	P1OUT |= 0x08;   //Enable pull up resistor

	P1IES |= 0x08;   //High to Low sensitivity
	P1IE  |= 0x08;   //Interrupt enabled
	P1IFG &= 0x08;  //Clearing any IRQ flag

	__bis_SR_register(LPM4_bits + GIE);
	while(1){
	}

}

#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void){
    P1OUT ^= 0x40;
    P1IFG &= ~0x08;
}
