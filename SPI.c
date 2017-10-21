#include <msp430.h>
#include <stdint.h>
#include <pinout.h>

void SPIopen(){
    //Turn off UCB0 State Machine
    UCB0CTL1 = UCSWRST;

    P1SEL |= BIT5 + BIT7 ; // P1.5 = SPI_Clk, P1.7=MOSI
    P1SEL2 |= BIT5 +  BIT7; // P1.5 = SPI_Clk, P1.7=MOSI

    P2SEL &= ~ADF_CS;
    P2SEL2 &= ~ADF_CS;
    P2DIR |= ADF_CS;
    P2OUT |= ADF_CS; //Active when CS goes low


    //10101000 = 0xA8
    UCB0CTL0 |= 0xA8;
    //11000000 = 0xC1
    UCB0CTL1 |= 0xC1;

    //16MHz/200 = 80kHz clock frequency
    UCB0BR0 = 200;
    UCB0BR1 = 0;

    //Turn on UCB0 State Machine
    UCB0CTL1 &= ~UCSWRST;
}

void SPIclose(){

}

void SPIread(){

}

void SPIwrite(uint16_t part1, uint16_t part2, uint16_t part3){
    while(!(IFG2 & UCB0TXIFG));
    P2OUT &= ~ADF_CS;
    UCB0TXBUF = part1;
    while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
    UCB0TXBUF = part2;
    while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
    UCB0TXBUF = part3;
    while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
    P2OUT ^= ADF_CS;
}

void SPImain(){

}
