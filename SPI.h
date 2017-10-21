#ifndef SPI_H
#define SPI_H

#include<stdint.h>

void SPIopen();
void SPIclose();
void SPIread();
void SPIwrite(uint16_t part1, uint16_t part2, uint16_t part3);
void SPImain();

#endif
