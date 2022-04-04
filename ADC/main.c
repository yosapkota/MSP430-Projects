#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

long temp;
long IntDegF;
long IntDegC;

long value;
long x;
int i;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;         // Stop WDT

  P1DIR |= BIT4;
  P1DIR |= 0x01;
  DCOCTL  = 0;                      // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;            // Set MCLK and SMCLK to 1MHz
  DCOCTL  = CALDCO_1MHZ;            // Set MCLK and SMCLK to 1MHz

                                    ////// Serial port back channel setup

  P1SEL  = BIT1 + BIT2;             // Select UART as the pin function
  P1SEL2 = BIT1 + BIT2;
  UCA0CTL1 |= UCSWRST;              // Disable UART module for configuration
  UCA0CTL0 = 0x00;                  // No parity, LSB first, 8-bit data, 1 stop bit, UART, Asynchronous
  UCA0CTL1 = UCSSEL_2 + UCSWRST;    // SMCLK source, keep in reset state
  UCA0BR0 = 104;                    // 9600 Baud rate   - Assumes 1 MHz clock
  UCA0BR1 = 0;                      // 9600 Baud rate   - Assumes 1 MHz clock
  UCA0MCTL = 0x02;                  // 2nd Stage modulation = 1, Oversampling off
  IE2 = 0x00;                       // Interrupts disabled
  UCA0CTL1 &= ~UCSWRST;
                                    // Analog setup
  ADC10CTL1 = INCH_10 + ADC10DIV_3; // Temperature Sensor and ADC10CLK/4
                                    // ADC set up parameters
  ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;
  __delay_cycles(10000);              // required for settling of ADC

while(1)
  {
  ADC10CTL0 |= ENC + ADC10SC;       // Sampling and conversion start with interrupt
  __bis_SR_register(CPUOFF + GIE);  // LPM0 with interrupts enabled, sleep until interrupt

  value = ADC10MEM;
  x  = (value/1023) * 5;
  serial_string("Value = ");
  serial_number(x);           //print value
  serial_string(" \n");

  P1DIR ^= 0x01;
  for(i =10000; i>0; i--);
  }
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
__bic_SR_register_on_exit(CPUOFF); // Clear CPUOFF bit from 0(SR)
}


// The following functions allow us to write strings and numbers to our PC through the back channel uart.
void serial_charTX(char c)
{
    while( !(IFG2 & UCA0TXIFG) );       // Wait until the transmit buffer is empty
    UCA0TXBUF = c;                      // Send the character through the Xmit buffer
}

char serial_charRX(void)
{
    while( !(IFG2 & UCA0RXIFG) );       // Wait until a character has been received
    return UCA0RXBUF;                   // Return received character
}

void serial_string(char string[])
{
    int i;
    for(i = 0; string[i] != '\0'; i++)  // Send characters until end-of-string
    {
        if(string[i] == '\n')           // The terminal program recognizes \r as the carriage return
            serial_charTX('\r');
        else
            serial_charTX(string[i]);
    }
}

void serial_number(int value)
{
    char string[10];

    sprintf(string, "%d", value);       // Convert an integer to a character string

    serial_string(string);
}

