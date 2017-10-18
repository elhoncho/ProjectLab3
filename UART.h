#ifndef UART_H
#define UART_H

int UART_Open();
int UART_Close();
int UART_Write(char outChar);
char UART_Read();
void UART_Main();

#endif
