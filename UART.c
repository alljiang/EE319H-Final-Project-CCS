
/*
 *  ======== uartecho.c ========
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include "Board.h"

#include <stdint.h>
#include "UART.h"

UART_Handle uart;
UART_Params uartParams;

void UART_start(void) {
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }
}

void UART_transmit(char msg[]) {
    UART_write(uart, msg, sizeof(msg));
}

void UART_receive(char* buffer, uint32_t bytesToRead) {
    UART_read(uart, &buffer, bytesToRead);
}
