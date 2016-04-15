#include <msp430.h> 

/*
 *	DroneSignalPusher
 *	by Bryan Bustos
 *
 *	Takes in 4 bytes of numbers through serial communication (Asynch UART)
 *	from Beaglebone Black
 *
 *	Translates those 4 bytes into time for pin's output to be high
 *	In: 4 bytes of data
 *	Out: 4 pins outputting 1 "channel" at a time
 *
 *	Consider: timing of total wavelength, and how clk speed affects it
 *
 *	Uses: SCLK, UART
 *
 *	Input: P1.1, P1.2 (UART)
 *
 *	Output: P1.4, P1.5, P1.7, P1.6
 *
*/
int read();
void decode();

unsigned int packet[9];
unsigned int pwnum[4];
int check;
int wavelength;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
    DCOCTL = CALDCO_16MHZ;
    P1SEL = BIT1 + BIT2 + BIT4;               // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2;                     // P1.4 = SMCLK, others GPIO

	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0BR0 = 138;                            // 16MHz 115200
	UCA0BR1 = 0;                              // 16MHz 115200
	UCA0MCTL = UCBRS_7 + UCBRF_0;             // 16MHz
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

	//0111 1000 = 0x71
	P1DIR |= 0x71; //Sets Pins 1.4, 1.5, 1.6, and 1.7 to outputs
	check=0;
	while(1){
		check = read();
		while(check!=0){
			check = read();
		}
		decode();
		//Output correctly timed for each signal
		//Turn pw[] values into usable numbers based on how much they need the signal to be on
		//97 cycles = 200 us = .2ms
		//Between channels, there is a .800ms delay
		//There are 5 delays, 5* .800ms = 4ms.
		//4ms * 97cycles/.2ms = 1940 cycles delay
		//Length of delay between packets is 16333 clock cycles
		//Add them all together to get the wavelength of the packet
		wavelength = pw[0]+pw[1]+pw[3]+pw[4]+16333+1940;
		for(int w=0; w<wavelength; w++){
			if(pw[0]>0){
				P1OUT=BIT4;
				pw[0]=pw[0]-1;
			}
			else{
				P1OUT &= ~(BIT4);
			}

			if(pw[1]>0){
				P1OUT=BIT5;
				pw[1]=pw[1]-1;
			}
			else{
				P1OUT &= ~(BIT5);
			}

			if(pw[2]>0){
				P1OUT=BIT6;
				pw[2]=pw[2]-1;
			}
			else{
				P1OUT &= ~(BIT6);
			}

			if(pw[3]>0){
				P1OUT=BIT7;
				pw[3]=pw[3]-1;
			}
			else{
				P1OUT &= ~(BIT7);
			}

		}
		//And repeat
	}
	
	return 0;
}

/*
 * read by BB
 * type: int, returns int
 * input: none
 * output: 0 if packet data undamaged, 1 if packet data damaged
*/
int read(){
	int checksum=0;
	for(int pn=0; pn<9; pn++){
			while (!(IFG2&UCA0RXIFG));                // USCI_A0 TRX buffer ready?
			packet[pn]=UCA0RXBUF;
	}
	for(int n=0; n<8; n++){
		checksum ^= packet[n];
	}
	if(checksum!=packet[8])
		return 1;
	return 0;
}

void decode(){
	for(int i=0; i<4; i++){
		pw[i] = (packet[(2*i)] * 256) + (packet[(2*i)+1])
	}
}
