#include <msp430.h>

void SPIopen(){
    //Turn off UCB0 State Machine
    UCB0CTL1 = UCSWRST;

    P1SEL |= BIT5 + BIT7 ; // P1.5 = SPI_Clk, P1.7=MOSI
    P1SEL2 |= BIT5 +  BIT7; // P1.5 = SPI_Clk, P1.7=MOSI

    P2SEL &= ~BIT5;
    P2SEL2 &= ~BIT5;
    P2DIR |= BIT5;
    P2OUT |= BIT5; //Active when CS goes low


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

void SPIwrite(){
    while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        P2OUT &= ~BIT5;
        UCB0TXBUF = 0x00;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        UCB0TXBUF = 0x30;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        UCB0TXBUF = 0xB3;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        P2OUT ^= BIT5;

        while(!(IFG2 & UCB0TXIFG));
        P2OUT &= ~BIT5;
        UCB0TXBUF = 0x00;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        UCB0TXBUF = 0x30;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        UCB0TXBUF = 0xB2;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        P2OUT ^= BIT5;

        while(!(IFG2 & UCB0TXIFG));
        P2OUT &= ~BIT5;
        UCB0TXBUF = 0x00;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        UCB0TXBUF = 0x00;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        UCB0TXBUF = 0x04;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        P2OUT ^= BIT5;

        while(!(IFG2 & UCB0TXIFG));
        P2OUT &= ~BIT5;
        UCB0TXBUF = 0x00;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        UCB0TXBUF = 0x01;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        UCB0TXBUF = 0x01;
        while(!(IFG2 & UCB0TXIFG) || (UCB0STAT & UCBUSY));
        P2OUT ^= BIT5;
}

void SPImain(){

}
