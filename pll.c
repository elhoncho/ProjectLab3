#include <stdint.h>
#include <pll.h>
#include <SPI.h>
#include <pinout.h>
#include <msp430.h>
#include <terminal.h>
#include <stdlib.h>
#include <string.h>

static uint16_t maxClockDevider = 16383;
static uint16_t maxLoopDevider = 8191;
static float refFreqMHz = 24;

static uint8_t rLatch0 = 0x04;
static uint8_t rLatch1 = 0x00;
static uint8_t rLatch2 = 0x00;

static uint8_t nLatch0 = 0x01;
static uint8_t nLatch1 = 0x07;
static uint8_t nLatch2 = 0x00;

static void BuildRefLatch(uint16_t rDevide);
static void BuildNLatch(uint16_t nDevide);

extern uint8_t debug;

void PLL_Open(int16_t maxClkDevide, int16_t maxLoopDevide, float referenceFreqMHz){
    maxClockDevider = maxClkDevide;
    maxLoopDevider = maxLoopDevide;
    refFreqMHz =referenceFreqMHz;
}

void PLL_Close(){

}

void PLL_Read(){

}

void PLL_Write(int freqMHz, int freqDecimal){
    uint16_t bestLoopDevide = 65535;
    uint16_t bestRefDevide = 65535;

    /*
    float bestDelta = 32000;
    float previousDelta = 32000;
    float testDelta = 32000;

    for(int i = 1; i < maxClockDevider; i++){
        for(int j = 1; j < maxLoopDevider; j++){
            previousDelta = testDelta;
            testDelta = (refFreqMHz/i) - (freqMHz/j);
            if(testDelta < 0){
                testDelta = testDelta*-1;
            }
            if(testDelta < bestDelta){
                bestDelta = testDelta;
                bestLoopDevide = j;
                bestRefDevide = i;
            }
            if(bestDelta < .001){
                break;
            }
        }
        if(bestDelta < .001){
            break;
        }
    }
    */

    bestRefDevide = 240;
    bestLoopDevide = freqMHz*10+freqDecimal;

    BuildRefLatch(bestRefDevide);
    BuildNLatch(bestLoopDevide);

    if(debug){
        char tmpStr[25] = "\r\nReference Devider:\r\n";
        terminalWrite(tmpStr);
        itoa(bestRefDevide, tmpStr, 10);
        terminalWrite(tmpStr);

        strcpy(tmpStr,"\r\nLoop Devider:\r\n");
        terminalWrite(tmpStr);
        itoa(bestLoopDevide, tmpStr, 10);
        terminalWrite(tmpStr);

        strcpy(tmpStr,"\r\n>");
        terminalWrite(tmpStr);
    }

    P2OUT &= ~ADF_ENABLE;
    //Function Latch
    SPIwrite(0x00, 0x30, 0x92);
    //R load
    SPIwrite(rLatch2, rLatch1, rLatch0);
    //N load
    SPIwrite(nLatch2, nLatch1, nLatch0);
    P2OUT ^= ADF_ENABLE;
}

static void BuildRefLatch(uint16_t rDevide){
     rLatch2 = 0x00;
    //Shift rDevide right by 2 to put in the 00 control bits, and with 0xFF00 to select middle 8 bits, shift right 8 bits to get middle 8 bits
    rLatch1 = (uint8_t)(((rDevide << 2) & 0xFF00) >> 8);
    rLatch0 = (uint8_t)((rDevide<< 2) & 0x00FF);
}

static void BuildNLatch(uint16_t nDevide){
    nLatch0 = 0x01;
    nLatch1 = (uint8_t)(nDevide & 0x00FF);
    nLatch2 = (uint8_t)((nDevide & 0xFF00) >> 8);
}
