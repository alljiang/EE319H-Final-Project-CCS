
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

void UART_transmit(uint8_t numBytes, uint8_t *buffer) {
    UART_write(uart, buffer, numBytes);
}

void UART_receive(uint32_t bytesToRead, uint8_t* buffer) {
    UART_read(uart, &buffer, bytesToRead);
}

void UART_sendAnimation(struct SpriteSendable sendable) {
    uint8_t animation_charIndex = sendable.charIndex;
    uint8_t animation_animationIndex = sendable.animationIndex;
    int16_t x = sendable.x;
    int16_t y = sendable.y;
    uint8_t frame = sendable.frame;
    uint8_t persistent = sendable.persistent;
    uint8_t layer = sendable.layer;
    uint8_t continuous = sendable.continuous;
    uint8_t framePeriod = sendable.framePeriod;
    uint8_t mirrored = sendable.mirrored;

    UARTBuffer[0] = animation_charIndex;
    UARTBuffer[1] = animation_animationIndex;
    UARTBuffer[2] = (x << 8) & 0xFF;
    UARTBuffer[3] = (x) & 0xFF;
    UARTBuffer[4] = (y << 8) & 0xFF;
    UARTBuffer[5] = (y) & 0xFF;
    UARTBuffer[6] = frame;
    UARTBuffer[7] = persistent;
    UARTBuffer[8] = layer;
    UARTBuffer[9] = continuous;
    UARTBuffer[10] = framePeriod;
    UARTBuffer[11] = mirrored;

    UART_transmit(12, UARTBuffer);
}

void UART_readCharacterSDCard(uint8_t characterIndex) {
//  TODO
}
