
/*
 *  ======== uartecho.c ========
 */

/* XDCtools Header files */
#include <driver/Board.h>
#include <driver/UART.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include <stdint.h>

UART_Handle uart;
UART_Params uartParams;

uint8_t UARTBuffer[100];

void readFunction(UART_Handle handle, uint8_t *buf, size_t count) {
}

void UART_start(void) {
    UART_Params_init(&uartParams);
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.readCallback = &readFunction;

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART1, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }
}

void UART_transmit(uint8_t *buffer) {
    UART_write(uart, buffer, sizeof(msg));
}

void UART_receive(char* buffer, uint32_t bytesToRead) {
    UART_read(uart, &buffer, bytesToRead);
}
