#include <msp430.h>
#include <terminal.h>
#include <UART.h>
#include <string.h>
#include <SPI.h>
#include <pinout.h>
#include <pll.h>
#include <stdlib.h>
#include <stdint.h>

extern uint8_t debug;

int volatile newCmd = 0;

typedef void(*functionPointerType)(char *arg1, char *arg2);
struct commandStruct{
    char const *name;
    functionPointerType execute;
    char const *help;
};

static void CmdClear(char *arg1, char *arg2);
static void AdfWrite(char *arg1, char *arg2);
static void FrequencyChange(char *arg1, char *arg2);
static void ManualFrequencyChange(char *arg1, char *arg2);
static void ManualClockChange(char *arg1, char *arg2);
static void ListCommands(char *arg1, char *arg2);
static void ToggelDebug(char *arg1, char *arg2);

const struct commandStruct commands[] ={
    {"clear", &CmdClear, "Clears the screen"},
    {"init", &AdfWrite, "Initializes the ADF4002"},
    {"freq", &FrequencyChange, "Change PLL Frequency"},
    {"regf", &ManualFrequencyChange, "Manual Set Frequency Reg"},
    {"regc", &ManualClockChange, "Manual Set Ref(Clk) Reg"},
    {"ls", &ListCommands, "Run Help Function"},
    {"help", &ListCommands, "Run Help Function"},
    {"debug", &ToggelDebug, "Toggels Debug Mode"},
    {"",0,""} //End of commands indicator. Must be last.
};

static void ToggelDebug(char *arg1, char *arg2){
    if(debug){
        debug = 0;
    }
    else{
        debug = 1;
    }
}

static void ListCommands(char *arg1, char *arg2){
    char tmpStr[40];
    uint8_t i = 0;
    while(commands[i].execute){
        strcpy(tmpStr, commands[i].name);
        terminalWrite(tmpStr);
        strcpy(tmpStr, " - ");
        terminalWrite(tmpStr);
        strcpy(tmpStr, commands[i].help);
        terminalWrite(tmpStr);
        strcpy(tmpStr,"\r\n");
        terminalWrite(tmpStr);
        i++;
    }
}
static void CmdClear(char *arg1, char *arg2){
    char tmpStr[25];
    strcpy(tmpStr, "\033[2J\033[;H");
    terminalWrite(tmpStr);
}

static void AdfWrite(char *arg1, char *arg2){
    //Init Latch
    SPIwrite(0x00, 0x30, 0x93);
    //Function Latch
    SPIwrite(0x00, 0x30, 0x92);
    //R load
    SPIwrite(0x00, 0x00, 0x04);
    //N load
    SPIwrite(0x00, 0x06, 0x01);
}

static void FrequencyChange(char *arg1, char *arg2){
    PLL_Write(atoi(arg1), atoi(arg2));
}

static void ManualFrequencyChange(char *arg1, char *arg2){
    PLL_ManualFrequency(atoi(arg1));
}

static void ManualClockChange(char *arg1, char *arg2){
    PLL_ManualClock(atoi(arg1));
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
        char tmpStr[25];
        static int i = 0;
        char tmpChar;

        char arg[3][22];

        tmpChar = UART_Read();

        arg[0][0] = '\0';
        arg[1][0] = '\0';
        arg[2][0] = '\0';

        i = 0;
        while(tmpChar != (char)-1){
            int len = strlen(arg[i]);

            //Dont store \r or \n or space or .
            if(tmpChar != 0x0D && tmpChar != 0x0A && tmpChar != 0x20 && tmpChar != 0x2E){
                arg[i][len] = tmpChar;
                arg[i][len+1] = '\0';
            }
            //space or . => new argument
            else if(tmpChar == 0x20 ||  tmpChar == 0x2E){
                i++;
            }

            tmpChar = UART_Read();
        }

        if(debug){
            strcpy(tmpStr, "\r\nArg0: \r\n");
            terminalWrite(tmpStr);
            terminalWrite(arg[0]);

            strcpy(tmpStr, "\r\nArg1: \r\n");
            terminalWrite(tmpStr);
            terminalWrite(arg[1]);

            strcpy(tmpStr, "\r\nArg2: \r\n");
            terminalWrite(tmpStr);
            terminalWrite(arg[2]);
        }

        i = 0;
        if(strlen(arg[0]) >= 1){
            while(commands[i].execute){
                if(strcmp(arg[0], commands[i].name) == 0){
                    strcpy(tmpStr, "\r\n");
                    terminalWrite(tmpStr);

                    commands[i].execute(arg[1], arg[2]);
                    strcpy(tmpStr, ">");
                    terminalWrite(tmpStr);
                    i = 0;
                    break;
                }
                i++;
            }
            //i is set to 0 if a command is found
            if(i != 0){
                strcpy(tmpStr, "\r\n");
                terminalWrite(tmpStr);

                strcpy(tmpStr, "No Command Found \r\n>");
                terminalWrite(tmpStr);
            }
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
