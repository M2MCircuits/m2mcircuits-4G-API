/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430G2xx3 Demo - USCI_A0, 115200 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1MHz = 1MHz/115200 = ~8.7
//   ACLK = n/a, MCLK = SMCLK = CALxxx_1MHZ = 1MHz
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |     P1.2/UCA0TXD|------------>
//            |                 | 115200 - 8N1
//            |     P1.1/UCA0RXD|<------------
//
//   D. Dang
//   Texas Instruments Inc.
//   February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include <msp430.h>
//should be #include <msp430g2553.h> ??

//Maker Stuff
#define firstCommand "ATE1" //4 times
#define secondCommand "AT"  // 2 times
unsigned int bufferindex;
unsigned char recieved_data_buffer_bytes [30];
//end Maker Stuff
extern void sayHi();
int main(void)
{
    	//start Maker Modem stuff
	bufferindex = 0;
	//end Maker Modem Stuff

	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	if (CALBC1_1MHZ==0xFF)			// If calibration constant erased
	{
	while(1);                               // do not load, trap CPU!!
	}//this makes sure the dco (built in internal oscillator) is running at the correct clock frequency


	DCOCTL = 0;                               // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
	DCOCTL = CALDCO_1MHZ;
	
	P1SEL = BIT1 + BIT2;// + BIT4;               // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2;                     // P1.4 = SMCLK, others GPIO
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0BR0 = 8;                              // 1MHz 115200
	UCA0BR1 = 0;                              // 1MHz 115200
	UCA0MCTL = UCBRS2 + UCBRS0;               // Modulation UCBRSx = 5
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
		  UCA0TXBUF = 'A';                    		// take the data byte and transmit out UART at 115kbaud
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
		  UCA0TXBUF = 'T';                    		// take the data byte and transmit out UART at 115kbaud
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
		  UCA0TXBUF = 'E';                    		// take the data byte and transmit out UART at 115kbaud
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
		  UCA0TXBUF = '1';                    		// take the data byte and transmit out UART at 115kbaud

	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
		  UCA0TXBUF = 'A';                    		// take the data byte and transmit out UART at 115kbaud
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
		  UCA0TXBUF = 'T';                    		// take the data byte and transmit out UART at 115kbaud

	while(1);
	//__bis_SR_register(LPM0_bits + GIE);       // Enter Low Power Mode 0, interrupts enabled
	__bis_SR_register(GIE);       // interrupts enabled

	sayHi();
}

// Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)

#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)

#else
#error Compiler not supported!
#endif
{//when interrupt is recieved, these lines run
  //while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
  //UCA0TXBUF = UCA0RXBUF;                    // TX -> RXed character

  //do things with recieved message from MakerModem
  //create a buffer that holds all the bytes that come back
  //capture UCA0RXBUF which is one byte
  recieved_data_buffer_bytes[bufferindex]=UCA0RXBUF;
  bufferindex++;
}

/*
 * 		CODE FOR MAKER MODEM BELOW THIS LINE
 */

#define LAUNCHPAD
#include <stdio.h>
#include <stdbool.h>
#include "cModemStream.h"
#include "cModemInterface.h"
#include "cSMS.h"
#include "cInet.h"

struct ModemStream* modem;
struct ModemInterface* interface;

extern void stringToByteSend(const unsigned char *string);
void sendStrings();

void sayHi(){
	sendStrings();
}
void sendByte(unsigned char data){
	//
	  while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
	  UCA0TXBUF = data;                    		// take the data byte and transmit out UART at 115kbaud
	//
}
void stringToByteSend(const unsigned char *string){
		//send those bytes through sendByte
		unsigned int i;
		for(i=0; i<sizeof(string); i++){
			sendByte(string[i]);
		}
}

void sendStrings(){
	/*int command_to_send = 1; //HERE WE SAY WHICH COMMAND TO SEND
	const unsigned char command[30];

	switch(command_to_send){
		case 1: command = firstCommand; //reboot
	}*/

	stringToByteSend(firstCommand);
	stringToByteSend(secondCommand);
}

//build and transmit command packets over to the MakerModem for sending a text
