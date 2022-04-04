#include <msp430.h>
#include "serial_msp.h"
 //   unsigned int i;
   // const char string[] = { "Transmitting\r\n" };
volatile unsigned char c;
int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P1DIR |= 0x41;
  P1REN |= 0x08;
  P1OUT |= 0x08;
  DCOCTL  = 0;             // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;   // Set range
  DCOCTL  = CALDCO_1MHZ;   // Set DCO step + modulation
  P1SEL  = BIT1 + BIT2;             // Select UART as the pin function
  P1SEL2 = BIT1 + BIT2;

  UCA0CTL1 |= UCSWRST;                // Disable UART module for configuration

  UCA0CTL0 = 0x00;                    // No parity, LSB first, 8-bit data, 1 stop bit, UART, Asynchronous
  UCA0CTL1 = UCSSEL_2 + UCSWRST;      // SMCLK source, keep in reset state
  UCA0BR0 = 104;                      // 9600 Baud rate   - Assumes 1 MHz clock
  UCA0BR1 = 0;                        // 9600 Baud rate   - Assumes 1 MHz clock
  UCA0MCTL = 0x02;                    // 2nd Stage modulation = 1, Oversampling off

  UCA0CTL1 &= ~UCSWRST;               // Enable UART module
  P1IE |= BIT3;
  P1IFG &= ~BIT3;
  IE2 |= UCA0RXIE;               // Enable USCI_A0 TX/RX interrupt
  IFG2 &= ~UCA0RXIE;
  P1OUT &= ~0x41;

  __bis_SR_register(LPM3_bits + GIE);
  while(1){
      {};
  }

}
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    //Enter your interrupt service routine here
    if(!((P1IN & 0x08) == 0x08))
        //serial_charTX('b');
        UCA0TXBUF = 'b';
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    {
   //Enter your interrupt service routine here
    while(!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = UCA0RXBUF;

    if (UCA0RXBUF == 'B')
        P1OUT |= 0x41;
    else if (UCA0RXBUF == 'b')
        P1OUT &= ~0x41;
    if (UCA0RXBUF == 'G')
        P1OUT |= 0x01;
    else if (UCA0RXBUF == 'g')
        P1OUT &= ~0x01;
    if (UCA0RXBUF == 'R')
        P1OUT |= 0x40;
    else if (UCA0RXBUF == 'r')
        P1OUT &= ~0x40;
    }
}
