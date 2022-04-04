/*
 * serial_msp.c
 *
 *  Created on: Jun 1, 2012
 *      Author: Erioto
 */
#include "serial_msp.h"

void serial_init(void)
{
    P1SEL  = BIT1 + BIT2;				// Select UART as the pin function
    P1SEL2 = BIT1 + BIT2;

    UCA0CTL1 |= UCSWRST;				// Disable UART module for configuration

		UCA0CTL0 = 0x00;					// No parity, LSB first, 8-bit data, 1 stop bit, UART, Asynchronous
		UCA0CTL1 = UCSSEL_2 + UCSWRST;		// SMCLK source, keep in reset state
	//	UCA0BR0 = 130;						// 9600 Baud rate	- Assumes 16 MHz clock
	//	UCA0BR1 = 6;						// 9600 Baud rate	- Assumes 16 MHz clock
		UCA0BR0 = 104;						// 9600 Baud rate	- Assumes 1 MHz clock
    	        UCA0BR1 = 0;						// 9600 Baud rate	- Assumes 1 MHz clock
		UCA0MCTL = 0x02;					// 2nd Stage modulation = 1, Oversampling off
		IE2 = 0x00;							// Interrupts disabled

	UCA0CTL1 &= ~UCSWRST;				// Enable UART module

}

void serial_charTX(char c)
{
    while( !(IFG2 & UCA0TXIFG) );		// Wait until the transmit buffer is empty
    UCA0TXBUF = c;						// Send the character through the Xmit buffer
}

char serial_charRX(void)
{
	while( !(IFG2 & UCA0RXIFG) );		// Wait until a character has been received
	return UCA0RXBUF;					// Return received character
}

void serial_string(char string[])
{
	int i;
	for(i = 0; string[i] != '\0'; i++)		// Send characters until end-of-string
	{
		if(string[i] == '\n')		// The terminal program recognizes \r as the carriage return
			serial_charTX('\r');
		else
			serial_charTX(string[i]);
	}
}

void serial_number(int value)
{
	char string[10];

	sprintf(string, "%d", value);		// Convert an integer to a character string

	serial_string(string);
}
