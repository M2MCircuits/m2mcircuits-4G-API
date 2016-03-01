/*
 * custombaud.h
 *
 *  Created on: Feb 29, 2016
 *      Author: Josh
 *      
 *  Reference: http://embedded-funk.net/writing-c-code-smartly-1-using-your-c-preprocessor-as-your-calculator/
 *  
 *  Usage in main:
 *  	// setup USCI UART registers
		UCA0CTL1 |= UCSSEL_2 + UCSWRST;
		UCA0BR0 = USCI_BR0_VAL;
		UCA0BR1 = USCI_BR1_VAL;
		UCA0MCTL = USCI_BRS_VAL;
		UCA0CTL1 &= ~UCSWRST;
 */

#ifndef CUSTOMBAUD_H_
#define CUSTOMBAUD_H_

#define USCI_INPUT_CLK      (8000000UL)  // in Hz
#define USCI_BAUD_RATE      (115200)

//automatically calculate the baud rate setting based on both inputs above (input clock frequency and descried baud rate)
#define USCI_DIV_INT              (USCI_INPUT_CLK/USCI_BAUD_RATE)
#define USCI_BR0_VAL              (USCI_DIV_INT & 0x00FF)
#define USCI_BR1_VAL              ((USCI_DIV_INT >> 8) & 0xFF)

#define USCI_DIV_FRAC_NUMERATOR   (USCI_INPUT_CLK - (USCI_DIV_INT*USCI_BAUD_RATE))
#define USCI_DIV_FRAC_NUM_X_8     (USCI_DIV_FRAC_NUMERATOR*8)
#define USCI_DIV_FRAC_X_8         (USCI_DIV_FRAC_NUM_X_8/USCI_BAUD_RATE)

#if (((USCI_DIV_FRAC_NUM_X_8-(USCI_DIV_FRAC_X_8*USCI_BAUD_RATE))*10)/USCI_BAUD_RATE < 5)
#define USCI_BRS_VAL              (USCI_DIV_FRAC_X_8<< 1)
#else
#define USCI_BRS_VAL              ((USCI_DIV_FRAC_X_8+1)<< 1)
#endif


#endif /* CUSTOMBAUD_H_ */
