
/*
 *  ======== uartecho.c ========
 */

/* XDCtools Header files */
#include <driver/Board.h>
#include <driver/UART.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include "metadata.h"

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include <stdint.h>
#include <stdbool.h>

UART_Handle uart;
UART_Params uartParams;
volatile bool UARTReceived = true;

#define UART_BUFFER_SIZE 100
uint8_t UARTBuffer[UART_BUFFER_SIZE];
uint8_t bufferStartIndex = 0;
uint8_t bufferSize = 0;

void readFunction(UART_Handle handle, uint8_t *buf, size_t count) {
    UARTReceived = true;
}

void UART_start(void) {
    UART_Params_init(&uartParams);
//    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeMode = UART_MODE_BLOCKING;
//    uartParams.readCallback = &readFunction;

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 5000000;
    uart = UART_open(Board_UART1, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }
}

void UART_transmit(uint8_t numBytes, uint8_t *buffer) {
    UART_write(uart, buffer, numBytes);
}

void UART_receive(uint32_t bytesToRead, uint8_t* buffer) {
    UART_read(uart, buffer, bytesToRead);
}

void UART_sendByte(uint8_t byte) {
    UART_transmit(1, &byte);
}
