#include  <msp430g2553.h>

unsigned int currentMinutes, currentSeconds;

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT

	BCSCTL1 |= DIVA_3;				// ACLK/8
	BCSCTL3 |= XCAP_3;				//12.5pF cap- setting for 32768Hz crystal

	P1DIR |= BIT0;					// set P1.0 (LED1) as output
	P1OUT |= BIT0;					// P1.0 low

	currentMinutes = 0;
	currentSeconds = 0;

	TACCTL1 = CAP_
	TACTL = TASSEL_1 + ID_1 + MC_1;			// ACLK, /8, upmode

	_BIS_SR(LPM3_bits + GIE);			// Enter LPM3 w/ interrupt
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	P1OUT ^= BIT0;					// Toggle LED
}



