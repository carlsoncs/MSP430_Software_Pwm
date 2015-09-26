#include <msp430.h>


const long MAX = 500;
volatile unsigned int count;
int pwm1_val = 1;
int pwm2_val = 2;
int pwm3_val = 4;
short adc_channel = 0;
short pwm1_off = 0, pwm2_off = 0, pwm3_off = 0;

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;

  P1DIR |= BIT7 + BIT4 + BIT5;             // P1.2 to output
  P1OUT |= BIT7 + BIT4 + BIT5;             // P1.2 to TA0.1

	P1SEL |= BIT3 + BIT1 + BIT2;					// ADC input pin P1.3

  DCOCTL = 0;
  BCSCTL1 |= CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // BCSCTL1 |= DIVA_0;  // Basic Clock System Control Register 1, DIVA sets
  //                     // divider on the ACLK to 0.
  // BCSCTL3 |= LFXT1S_2;  // Sets the system to use VLO.
  // BCSCTL3 |= XCAP_0; // Setting Capacitance to 0. (Experimental)

  IFG1 &= ~OFIFG;   // This clears the oscilator fault interrupt flag register

  // _BIS_SR(SCG1 + SCG0); // Turn off both SMCLK and DCO to save power.

  // BCSCTL2 |= SELM_3;  // Sets VLO clock as source for master clock.

  WDTCTL = WDT_MDLY_0_5;   // Sets WDT to use ACLK as a timer with an interval of
                          // 1.9 ms.
  // WDTCTL = WDT_MDLY_0_064;
  IE1 |= WDTIE;           // Enable Watchdog Timer Interrupt.
  count = 0;

ADC10CTL1 = INCH_3 + ADC10DIV_0 ;         // Channel 3, ADC10CLK/3

// Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on
ADC10CTL0 |= SREF_0 + ADC10SHT_2;// + REFON + REF2_5V;
ADC10CTL0 |= ADC10ON;
ADC10AE0 |= BIT3;
ADC10CTL0 |= ENC + ADC10SC;

  _BIS_SR(LPM0_bits + GIE); // Enable interrupts, ENTER LPM3

}



#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
  _BIC_SR(GIE); // Disable interrupts

  if(count % 13 == 0)
  {
    switch(adc_channel)
    {
      case 0:
      ADC10CTL1 &= ~INCH_2;
      ADC10CTL1 = INCH_3;
      ADC10AE0 &= ~BIT2;
      ADC10AE0 |= BIT3;
      break;

      case 1:
      ADC10CTL1 &= ~INCH_3;
      ADC10CTL1 = INCH_1;
      ADC10AE0 &= ~BIT3;
      ADC10AE0 |= BIT1;
      break;

      case 2:
      ADC10CTL1 &= ~INCH_1;
      ADC10CTL1 |= INCH_2;
      ADC10AE0 &= ~BIT1;
      ADC10AE0 |= BIT2;
      break;
    }
    ADC10CTL0 |= ENC + ADC10SC;
  }
  if(count >= MAX)
  {
    P1OUT |= BIT7 + BIT4 + BIT5; // Turn on power to all outputs
    pwm1_off = 0;
    pwm2_off = 0;
    pwm3_off = 0;
    count = 0; // and reset count.
  }
  else //if(!(pwm1_off + pwm2_off + pwm3_off == 3))
  {
    if( count > pwm1_val )
    {
      P1OUT &= ~BIT7; // Turn off power to this output.
      pwm1_off = 1;
    }
    if(count >= pwm2_val)
    {
      P1OUT &= ~BIT4;
      pwm2_off = 1;
    }
    if(count > pwm3_val)
    {
      P1OUT &= ~BIT5;
      pwm3_off = 1;
    }
  }

  if(count % 19 == 0 )
  {
    switch(adc_channel)
    {
      case 0:
      pwm1_val = (ADC10MEM);
      adc_channel = 1;
      break;

      case 1:
      pwm2_val = (ADC10MEM);
      adc_channel = 2;
      break;

      case 2:
      pwm3_val = (ADC10MEM);
      adc_channel = 0;
      break;
    }
    ADC10CTL0 &= ~(ENC + ADC10SC);
  }

  count++;

  _BIS_SR(GIE); // Reenable Interrupts.
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
}
