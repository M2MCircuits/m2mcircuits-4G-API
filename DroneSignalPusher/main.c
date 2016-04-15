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
unsigned int pw[4];
int check;
int wavelength;

/*
 * Main by BB
 * Initializes Clocks
 * Initializes UART
 * Initializes Input and Output Pins
 * Calls function to read the 9 byte packet coming in through UART
 * Decodes the Packet, turning into 4 numbers for the channel widths
 * Outputs the 4 channels seperately to be sent to the Pixhawk
 * via the PPM Encoder
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;                   //Calibrate Basic Clock Module to 16MHz
    DCOCTL = CALDCO_16MHZ;					  //Calibrate Digital Controlled Oscillator to 16MHz
    P1SEL = BIT1 + BIT2;  		              // P1.1 = RXD, P1.2=TXD for UART Serial Communication
	P1SEL2 = BIT1 + BIT2;

	UCA0CTL1 |= UCSSEL_2;                     // SMCLK selected for UART
	UCA0BR0 = 138;                            // 16MHz 115200 - Baud Rate
	UCA0BR1 = 0;                              // 16MHz 115200
	UCA0MCTL = UCBRS_7 + UCBRF_0;             // 16MHz modulation set
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

	//0111 1000 = 0x71
	P1DIR |= 0x71; //Sets Pins 1.4, 1.5, 1.6, and 1.7 to outputs
	check=0; //Set the check flag to zero
	while(1){
		check = read(); //Reads in packet through UART
		//If the checksum is fine, check=0, and the packet is intact
		//Else, re-read until we get a usable packet
		while(check!=0){
			check = read();
		}
		//Now that we have a usable packet, we turn it into usable data
		decode();

		//Channels vary from 97 cycles minimum to 516 maximum
		//97 cycles = 200 us = .2ms
		//Between channels, there is a .800ms delay
		//There are 5 delays, 5* .800ms = 4ms.
		//4ms * 97cycles/.2ms = 1940 cycles delay
		//Length of delay between packets is 16333 clock cycles
		//Add them all together to get the wavelength of the channel signal
		wavelength = pw[0]+pw[1]+pw[3]+pw[4]+16333+1940; //This will be modified as needed
		int w;
		for(w=0; w<wavelength; w++){ //For the length of the signal
			if(pw[0]>0){ //We check if we've output the entire channel yet
				P1OUT=BIT4; //If not, we set the output to high and decrement the value of the channel
				pw[0]=pw[0]-1;
			}
			else{//If it's done, we make sure the output is low
				P1OUT &= ~(BIT4);
			}
			//Repeat for the 4 channels
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
	int pn, n;
	for(pn=0; pn<9; pn++){ //Read in each byte of the backet
			while (!(IFG2&UCA0RXIFG));    // USCI_A0 RX buffer ready? check flags until it's ready
			packet[pn]=UCA0RXBUF;	//Save the byte to the packet array and repeat
	}
	for(n=0; n<8; n++){ //Now, we check the checksum to ensure the packet is fine
		checksum ^= packet[n]; //Checksum is the xor of the contents
	}
	if(checksum!=packet[8]) //If the calculated checksum doesn't equal the packet checksum
		return 1; //Return 1, it's not fine :(
	return 0; //Else, it's intact, return 0
}

/*
 * decode by BB
 * type: void, returns nothing
 * input: none, uses global variables
 * output: modifies the global variable pw[]
 *
 */
void decode(){
	int i;
	for(i=0; i<4; i++){ //For each of the 4 locations in the array
		pw[i] = (packet[(2*i)] * 256) + (packet[(2*i)+1]);
		//The channel lengths were min 97, max 516, so they had to be split into 2 bytes
		//B1 = X/256, B2=X%256
		//To put them together, we multiply byte1 by 256, and add byte2 as the remainder
		//Getting the original number of cycles to use in our outputs
	}
}
