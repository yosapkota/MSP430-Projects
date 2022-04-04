#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

volatile unsigned int count=0;
volatile unsigned int ms_delay=0;
int i;

void serial_string(char string[]);
void serial_number(int value);

unsigned int sec = 0;
unsigned int min = 0;
unsigned int hrs = 0;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  DCOCTL  = 0;             // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;   // Set range
  DCOCTL  = CALDCO_1MHZ;   // Set DCO step + modulation

  P1DIR |= 0x40;
  P1REN |= BIT3;
  P1OUT |= BIT3;
  P1IE |= BIT3;
  P1OUT &= ~0x40;
  P1IFG &= ~BIT3;

  TA0CCTL0 = CCIE;                    // CCR0 interrupt enabled
  TA0CCR0 = 1000;                     // 1ms Timer
  TA0CTL = TASSEL_2 + MC_1;           // SMCLK, contmode

  P1SEL  = BIT1 + BIT2;               // Select UART as the pin function
  P1SEL2 = BIT1 + BIT2;
  UCA0CTL1 |= UCSWRST;                // Disable UART module for configuration
  UCA0CTL0 = 0x00;                    // No parity, LSB first, 8-bit data, 1 stop bit, UART, Asynchronous
  UCA0CTL1 = UCSSEL_2 + UCSWRST;      // SMCLK source, keep in reset state
  UCA0BR0 = 104;                      // 9600 Baud rate   - Assumes 1 MHz clock
  UCA0BR1 = 0;                        // 9600 Baud rate   - Assumes 1 MHz clock
  UCA0MCTL = 0x02;                    // 2nd Stage modulation = 1, Oversampling off
  UCA0CTL1 &= ~UCSWRST;               // Enable UART module

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt

}

/////////////////////////////////////////////
// Timer A0 interrupt service routine
/////////////////////////////////////////////
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
{

    TA0CCR1 += 1;

    if(TA0CCR1 >= 1000){
        sec += 1;
        serial_string("Time :");
        serial_number(hrs);
        serial_string(" : ");
        serial_number(min);
        serial_string(" : ");
        serial_number(sec);
        serial_string("\n");
        TA0CCR1 = 0;
    }

    if(sec >= 60){
        min += 1;
        sec = 0;
    }

    if(min >= 60){
        hrs += 1;
        min = 0;
        sec = 0;
    }

}


void serial_string(char string[])
{
    int i;
    for(i = 0; string[i] != '\0'; i++)      // Send characters until end-of-string
    {
        while( !(IFG2 & UCA0TXIFG) );       // Wait until the transmit buffer is empty
        UCA0TXBUF = string[i];                      // Send the character through the Xmit buffer
    }
}

void serial_number(int value)
    {
        char string[10];
        sprintf(string, "%d", value);       // Convert an integer to a character string
        serial_string(string);
    }
