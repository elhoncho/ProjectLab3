#include <msp430.h>
#include <UART.h>
#include <terminal.h>
#include <string.h>
#include <SPI.h>
#include <pinout.h>
#include <pll.h>

uint8_t debug = 1;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    //Setup the clock
    DCOCTL = 0; // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ; // Set DCO to 16MHz
    DCOCTL = CALDCO_16MHZ; // Set DCO to 16MHz

	//Set all P1 and P2 Pins to output
	P1DIR |= 0xFF;
	P2DIR |= 0xFF;
	//Turn off all P1 and P2 Pins
	P1OUT &= 0x00;
	P2OUT &= 0x00;

	P2SEL &= ~ADF_ENABLE;
    P2SEL2 &= ~ADF_ENABLE;
    P2DIR |= ADF_ENABLE;
    P2OUT |= ADF_ENABLE; //Turn on ADF4002

    //Configure MUX in pin
    P2DIR &= ~BIT0; //Set pin to input
    P2REN ^= BIT0; //Enable pull up/down resistor
    P2OUT &= ~BIT0; //Pull Down Resistor
    P2SEL &= ~BIT0; //Set pin to dio
    P2SEL2 &= ~BIT0; //Set pin to dio

    __bis_SR_register(GIE); //Interrupts enabled

	UART_Open();
	SPIopen();
	terminalOpen();
	SPImain();
	PLL_Open(16383, 8191, 24);

	while(1)
	{
		//Loops to run
	    terminalMain();
	    UART_Main();
	}

	return 0;
}
