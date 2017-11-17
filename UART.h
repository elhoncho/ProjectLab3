#ifndef UART_H
#define UART_H

void UART_Open();
void UART_Close();
int UART_Write(char outChar);
char UART_Read();
void UART_Main();

#endif
