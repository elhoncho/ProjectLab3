#include <msp430.h>
#include <terminal.h>
#include <UART.h>
#include <string.h>
#include <SPI.h>
#include <pinout.h>

int volatile newCmd = 0;

typedef void(*functionPointerType)(void);
struct commandStruct{
    char const *name;
    functionPointerType execute;
    char const *help;
};

void CmdClear();
void AdfWrite();
void FrequencyChange();
void Lock();

const struct commandStruct commands[] ={
    {"clear", &CmdClear, "Clears the screen"},
    {"go", &AdfWrite, "Writes to the ADF4002"},
    {"freq", &FrequencyChange, "Change PLL Frequency"},
    {"lock", &Lock, "Mux out to PLL Lock"},
    {"",0,""} //End of commands indicator. Must be last.
};

void CmdClear(){
    char tmpStr[25];
    strcpy(tmpStr, "\033[2J\033[;H");
    terminalWrite(tmpStr);
    strcpy(tmpStr,">");
    terminalWrite(tmpStr);
}

void AdfWrite(){
    //Init Latch
    SPIwrite(0x00, 0x30, 0xA3);
    //Function Latch
    SPIwrite(0x00, 0x30, 0xA2);
    //R load
    SPIwrite(0x00, 0x00, 0x04);
    //N load
    SPIwrite(0x00, 0x06, 0x01);
}

void FrequencyChange(){
    P2OUT &= ~ADF_ENABLE;
    //Function Latch
    SPIwrite(0x00, 0x30, 0xC2);
    //R load
    SPIwrite(0x00, 0x00, 0x04);
    //N load
    SPIwrite(0x00, 0x06, 0x01);
    P2OUT ^= ADF_ENABLE;
}

void Lock(){
    P2OUT &= ~ADF_ENABLE;
    //Function Latch
    SPIwrite(0x00, 0x30, 0xCA);
    //R load
    SPIwrite(0x00, 0x00, 0x04);
    //N load
    SPIwrite(0x00, 0x06, 0x01);
    P2OUT ^= ADF_ENABLE;
}

void terminalOpen(){
    char tmpStr[25];
    strcpy(tmpStr, "\033[2J\033[;H");
    terminalWrite(tmpStr);
    strcpy(tmpStr,"Battle Control Online:");
    terminalWrite(tmpStr);
    strcpy(tmpStr,"\r\n>");
    terminalWrite(tmpStr);
}

void termianlClose(){

}

void terminalRead(){
    if(newCmd){
        static int i;
        char tmpChar;
        char tmpStr[] = "\r\nCommand Entered\r\n>";
        terminalWrite(tmpStr);

        tmpChar = UART_Read();

        tmpStr[0] = '\0';
        while(tmpChar != (char)-1){
            int len = strlen(tmpStr);

            //Dont store \r or \n
            if(tmpChar != 0x0D && tmpChar != 0x0A){
                tmpStr[len] = tmpChar;
                tmpStr[len+1] = '\0';
            }

            tmpChar = UART_Read();
        }

        i = 0;
        while(commands[i].name){
            if(strcmp(tmpStr, commands[i].name) == 0){
                commands[i].execute();
            }
            i++;
        }

        newCmd = 0;
    }
}

void terminalWrite(char *txStr){
    for(int i = 0; i < strlen(txStr); i++){
        UART_Write(txStr[i]);
    }
    UART_Main();
}

void terminalMain(){
    terminalRead();
}
