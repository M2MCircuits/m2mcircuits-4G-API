#include <msp430.h> 

#define HIGH 1
#define LOW  0

//*******  GLOBALS  *********************************************************
unsigned int pulse_widths[4];   //Hold values of pulse widths
unsigned char P15_high;
unsigned int resynch=0;
volatile unsigned char data, pulse_num;
volatile unsigned int pulse_time;
unsigned int packet[9];
volatile unsigned char data;
//*******  PROTOTYPES  ********************************************************
void init_port_pin_interrupt(void);
void init_clock(void);
void init_UART();
void synch();
void send();
void encode();
//***************************************************************************
// Port 1 interrupt service routine
// This function is called when an interrupt occurs on the P1 pins as specified
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    data = P1IN;
    if(data & BIT5) //if high
    {
    	P15_high = HIGH;//just went high so either starting 1 of 4 pulses, or starting pause between pulses
    }

    P1IFG &= ~BIT5;   // P1.5 IFG cleared, saying that the interrupt is done
}

//********************************************************************************
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;// Stop watchdog timer

	init_clock();
	init_port_pin_interrupt();
    __bis_SR_register(GIE);   //enable global interrupts
    init_UART();

    //SYNC
    //find the long pause before first of 4 pulses
    synch();

    //now ready to acquire 4 pulse times and send out serially during pause between pulses

    //ACQUIRE
    while(1)
	{
		for(pulse_num = 0; pulse_num < 4; pulse_num++)
		{
			P15_high = LOW;//clear flag
			while(P15_high == LOW);//while low - wait for start of a pulse
			pulse_time=0;
			while(P15_high == HIGH)//increment counter while high to get the pulse width
			{
				pulse_time++;
				data = P1IN;
				P15_high = LOW;
				if(data & BIT5) //if high
					P15_high = HIGH;
			}

			pulse_widths[pulse_num] = pulse_time;   //Hold values of Channel Length
		}

		//build packet with EX_OR checksum.................
		encode();
		//send packet at 115200 baud to MakerModem
		send();
		//Resynchs after every packet, as it becomes unreliable almost immediately
		//Due to the time taken to transfer and encode data
		synch();
		// if(resynch<=1){
		// 	resynch++;
		// }
		// else{
		// 	resynch=0;
		// 	synch();
		// }
	    //should probably repeat the above sync process periodically.............................
	}
}




//********************************************************************************
/*
 * 	init_UART()
 * 	type: void
 * 	input: none
 * 	output: none
 *
 * 	Initializes the serial communication in UART mode
 */
void init_UART()
{
	P1SEL = BIT1 + BIT2 + BIT4;               // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2;                     // P1.4 = SMCLK, others GPIO

	UCA0CTL1 |= UCSSEL_2;                     // Ties UART to SMCLK

	//For testing purposes
	//UCA0BR0 = 8;                            // 1MHz 115200
	//UCA0BR1 = 0;                            // 1MHz 115200
	//UCA0MCTL = UCBRS2 + UCBRS0;             // 1MHz Modulation UCBRSx = 5

	//Setting Baud Rates for Communication
	UCA0BR0 = 138;                            // 16MHz 115200
	UCA0BR1 = 0;                              // 16MHz 115200
	UCA0MCTL = UCBRS_7 + UCBRF_0;             // 16MHz

	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

	//IE2 |= UCA0RXIE;                        // Enable USCI_A0 RX interrupt, not needed
}

//******************************************************************************************************************
void init_port_pin_interrupt()
{
	  P1DIR &= ~BIT5;                           // Set P1.5 to input direction
	  P1IE |= BIT5;                             // P1.5 interrupt enabled
	  P1IES &= ~BIT5;                           // Set interrupt to occur when P1.5 detects rising edge
	  P1IFG &= ~BIT5;                           // P1.5 Interrupt FlaG cleared
}


//********************************************************************************
void init_clock()
{
	  if (CALBC1_16MHZ==0xFF)					// If calibration constant erased
	  {
	    while(1);                               // do not load, trap CPU, as things are wrong
	  }

	  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
	  BCSCTL1 = CALBC1_16MHZ;                    //Calibrate Basic Clock System Control to 16MHz
	  DCOCTL = CALDCO_16MHZ;					//Calibrate Digitalliy Controlled Oscillator to 16MHz
}


void synch(){
	 while(1) //repeat until the signal has been synched
	    {
			pulse_time = 0;

			P15_high = LOW;//clear flag, we are between pulses
			while(P15_high == LOW);//while low - wait for start of a pulse

			while(P15_high == HIGH)//increment counter while high to get the pulse width
			{
				pulse_time++; //increment counter
			    data = P1IN; //set data to be the data coming in
		    	P15_high = LOW; //set the flag to low
			    if(data & BIT5) //if high
			    	P15_high = HIGH; //set the flag back to high
			    //this way, if the flag isn't high, we break
			}

			//just went low for end of long pause between pulses
			if(pulse_time > 4000)//~16000 - if at the end of the pause between pulses
				break;
	    }

}

//Adding a checksum isn't done yet, figuring that out
void encode(){//Encodes Pulse Width Numbers into Bytes, and adds checksum
	//Reads signal data from pulse_widths[]
	//Saving endcoded data to global array packet[] for transmission
	int checksum, n, i;
	//Long Pause = 16000 Clock Cycles
	//Min Control = 97 clock cycles
	//Max Control = 619 clock cycles
	//Difference of 516 clock cycles
	//Encode in 2 bytes for each number

	//Break into 2 bytes: X/256, X%256, and send both values on
	//Then, other side puts both values together
	//Checksum will be ^= of each value altogether
	for(n=0; n<4; n++){
		packet[(2*n)] = pulse_widths[n] / 256;
		packet[((2*n)+1)]= pulse_widths[n] % 256;
	}
	//Checksum
	//XOR each byte to get checksum value
	for(i=0; i<8; n++){
		checksum ^= packet[i];
	}
	packet[8]=checksum;//save checksum to packet to be transferred
}

void send(){
	int p;
	for(p=0; p<9; p++){//Sets the 9 bytes over UART
		while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
		UCA0TXBUF = packet[p];
	}
}
