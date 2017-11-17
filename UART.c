#include <UART.h>
#include <msp430.h>


//Max amount of characters to buffer on the rx
#define BUFFER_LENGTH 25

extern int volatile newCmd;

struct circleBuffer{
    volatile unsigned int head;
    volatile unsigned int tail;
    volatile char buffer[BUFFER_LENGTH];
};

static struct circleBuffer rxBuffer;
static struct circleBuffer txBuffer;

static int pushToBuffer(struct circleBuffer *b, const char inChar);
static int popFromBuffer(struct circleBuffer *b, char *outChar);

void UART_Open(){
    /* Configure hardware UART */

    UCA0CTL1 |= UCSWRST;

    P1SEL |= BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2; // Use SMCLK

    UCA0BR0 = 0x82; // Set baud rate to 9600 with 16MHz clock (Family User Guide 15.3.13)
    UCA0BR1 = 0x06; // Set baud rate to 9600 with 16MHz clock
    UCA0MCTL = UCBRS2+UCBRS1; // Modulation UCBRSx = 6


    UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
    IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

    rxBuffer.head = 0;
    rxBuffer.tail = 0;
    txBuffer.head = 0;
    txBuffer.tail = 0;

}

void UART_Close(){

}

char UART_Read(){
    char rxChar;
    if(popFromBuffer(&rxBuffer, &rxChar) == 0){
        return rxChar;
    }
    return -1;
}

void UART_Main(){
    static char tmpTxChar;
    while(popFromBuffer(&txBuffer, &tmpTxChar) == 0){
        while(!(IFG2 & UCA0TXIFG));
        UCA0TXBUF = tmpTxChar;
    }
}

int UART_Write(char txChar){
    pushToBuffer(&txBuffer, txChar);
    return 0;
}

static int pushToBuffer(struct circleBuffer *b, const char inChar){
    if(b->head == BUFFER_LENGTH-1){
        b->head = 0;
    }
    else{
        b->head++;
    }

    if(b->head != b->tail){
        b->buffer[b->head] = inChar;
        return 0;
    }
    else{
        //Make sure head and tail are not both 0
        if(b->head == 0){
            b->head = BUFFER_LENGTH-1;
        }
        else{
            b->head--;
        }
        return -1;
    }
}

static int popFromBuffer(struct circleBuffer *b, char *outChar){
    if(b->tail != b->head){
        if(b->tail == BUFFER_LENGTH-1){
            b->tail = 0;
        }
        else{
            b->tail++;
        }

        *outChar = b->buffer[b->tail];
        return 0;
    }
    else{
        //Head equals tail, therefore nothing on the buffer
        return -1;
    }
}

__attribute__((interrupt(USCIAB0RX_VECTOR)))
void USCI0RX_ISR(void)
{
    char rxChar;
    rxChar = UCA0RXBUF;

    //Check to make sure inputString is an ASCII char and not null or CR or LF
    if(((rxChar & 0x80) == 0) && (rxChar != '\0') && (rxChar != 0x0D)  && (rxChar != 0x0A)){
        pushToBuffer(&rxBuffer, rxChar);
        pushToBuffer(&txBuffer, rxChar);
    }

    if(rxChar == 0x0D){
        newCmd = 1;
    }
}
