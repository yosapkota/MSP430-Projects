
#include <msp430.h>
#define WDT_meas_setting (DIV_SMCLK_512)
/* Defines WDT ACLK interval for delay between measurement cycles*/
#define WDT_delay_setting (DIV_ACLK_512)

/* Sensor settings*/
#define KEY_LVL     150                     // Defines threshold for a key press
/*Set to ~ half the max delta expected*/

/* Definitions for use with the WDT settings*/

#define DIV_ACLK_512    (WDT_ADLY_16)       // ACLK/512
#define DIV_SMCLK_512   (WDT_MDLY_0_5)      // SMCLK/512

// Global variables for sensing
unsigned int base_cnt, meas_cnt;
int delta_cnt;
char key_pressed;
int cycles;
/* System Routines*/
void measure_count(void);                   // Measures each capacitive sensor
/*declare your required function*/                       // LED gradient routine
void pulse_LED(void);
/* Main Function*/
int main(void)
{
  unsigned int i,j;
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
  DCOCTL =  CALDCO_1MHZ;
  BCSCTL3 |= LFXT1S_2;                      // LFXT1 = VLO

 // terminal readings
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
 //end here


  IE1 |= WDTIE;                             // enable WDT interrupt
  P2SEL = 0x00;                             // No XTAL
  P1OUT = 0x00;

  P1DIR = 0x41;                   // P1.0 & P1.6 = LEDs
  P1OUT &= ~0x41;

   __bis_SR_register(GIE);                  // Enable interrupts

  measure_count();                          // Establish baseline capacitance
    base_cnt = meas_cnt;

  for(i=15; i>0; i--)                       // Repeat and avg base measurement
  {
    measure_count();
      base_cnt = (meas_cnt+base_cnt)/2;
  }

  while (1)
  {
    j = KEY_LVL;
    key_pressed = 0;                        // Assume no keys are pressed

    measure_count();                        // Measure all sensors

    delta_cnt = base_cnt - meas_cnt;      // Calculate delta: c_change

    /* Handle baseline measurement for a base C decrease*/
    if (delta_cnt < 0)                    // If negative: result increased
    {                                     // beyond baseline, i.e. cap dec
      base_cnt = (base_cnt+meas_cnt) >> 1; // Re-average quickly
      delta_cnt = 0;                    // Zero out for pos determination
    }
    if (delta_cnt > j)                    // Determine if each key is pressed
    {                                     // per a preset threshold
      j = delta_cnt;
      key_pressed = 1;                    // key pressed
    }
    else
      key_pressed = 0;

    /* Delay to next sample, sample more slowly if no keys are pressed*/
    if (key_pressed)
    {
      BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_0; // ACLK/(0:1,1:2,2:4,3:8)
      cycles = 20;
    }
    else
    {
      cycles--;
      if (cycles > 0)
        BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_0; // ACLK/(0:1,1:2,2:4,3:8)
      else
      {
        BCSCTL1 = (BCSCTL1 & 0x0CF) + DIVA_3; // ACLK/(0:1,1:2,2:4,3:8)
        cycles = 0;
      }
    }
    WDTCTL = WDT_delay_setting;             // WDT, ACLK, interval timer

    /* Handle baseline measurement for a base C increase*/
    if (!key_pressed)                       // Only adjust baseline down
    {                                       // if no keys are touched
        base_cnt = base_cnt - 1;            // Adjust baseline down, should be
    }                                       // slow to accomodate for genuine

    __bis_SR_register(LPM3_bits);
    // Call your required function
        pulse_LED();
  }
}                                           // End Main

/* Watchdog Timer interrupt service routine*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
  TA0CCTL1 ^= CCIS0;                        // Create SW capture of CCR1
  __bic_SR_register_on_exit(LPM3_bits);     // Exit LPM3 on reti
}

/* Measure count result (capacitance) of each sensor*/
/* Routine setup for four sensors, not dependent on NUM_SEN value!*/

void measure_count(void)
{

  TA0CTL = TASSEL_3+MC_2;                   // TACLK, cont mode
  TA0CCTL1 = CM_3+CCIS_2+CAP;               // Pos&Neg,GND,Cap


    /*Configure Ports for relaxation oscillator*/
    /*The P2SEL2 register allows Timer_A to receive it's clock from a GPIO*/
    /*See the Application Information section of the device datasheet for info*/

    P1SEL2 |= BIT4;

    /*Setup Gate Timer*/
    WDTCTL = WDT_meas_setting;              // WDT, ACLK, interval timer
    TA0CTL |= TACLR;                        // Clear Timer_A TAR
    __bis_SR_register(LPM0_bits+GIE);       // Wait for WDT interrupt
    TA0CCTL1 ^= CCIS0;                      // Create SW capture of CCR1
    meas_cnt = TACCR1;                      // Save result
    WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer
}
void pulse_LED(void)
{
  /*Write your code here*/
    if(key_pressed)
        P1OUT ^= 0x41;
    else
        P1OUT &= ~0x41;
}
