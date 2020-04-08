
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

uint8_t UARTBuffer[50];
uint8_t UARTReceiveBuffer[4];
bool acknowledged;

void callbackFunction(UART_Handle handle, uint8_t *buf, size_t count) {
    acknowledged = true;
}

void UART_start(void) {
    UART_Params_init(&uartParams);
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.readCallback = &callbackFunction;

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART1, &uartParams);

    acknowledged = false;

    if (uart == NULL) {
        System_abort("Error opening UART");
    }
}

void UART_transmit(uint8_t numBytes, uint8_t *buffer) {
    UART_write(uart, buffer, numBytes);
}

void UART_receive(uint32_t bytesToRead, uint8_t* buffer) {
    UART_read(uart, buffer, bytesToRead);
}

void UART_waitForAcknowledge() {
    UART_receive(1, UARTReceiveBuffer);
}

void UART_sendAnimation(struct SpriteSendable sendable) {

    /*
     * Byte 0: 0x0B
     * Byte 1: Character Index
     * Byte 2: Animation Index
     * Byte 3: (x >> 8) & 0xFF
     * Byte 4: (x) & 0xFF
     * Byte 5: (y) & 0xFF
     * Byte 6: Frame Index
     * Byte 7: Persistent (boolean)
     * Byte 8: Layer
     * Byte 9: Continuous (boolean)
     * Byte 10: Frame Period
     * Byte 11: Mirrored (boolean)
     */

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

    UARTBuffer[0] = 0x0B;
    UARTBuffer[1] = animation_charIndex;
    UARTBuffer[2] = animation_animationIndex;
    UARTBuffer[3] = (x >> 8) & 0xFF;
    UARTBuffer[4] = (x) & 0xFF;
    UARTBuffer[5] = (y) & 0xFF;
    UARTBuffer[6] = frame;
    UARTBuffer[7] = persistent;
    UARTBuffer[8] = layer;
    UARTBuffer[9] = continuous;
    UARTBuffer[10] = framePeriod;
    UARTBuffer[11] = mirrored;

    while(!acknowledged) {}
    acknowledged = false;
    UART_transmit(12, UARTBuffer);
    UART_waitForAcknowledge();
}

void UART_commandUpdate() {
    /*
     * Byte 0: 0xFE
     */

    UARTBuffer[0] = 0xFE;

    while(!acknowledged) {}
    acknowledged = false;
    UART_transmit(1, UARTBuffer);
    UART_waitForAcknowledge();
}

void UART_readCharacterSDCard(uint8_t characterIndex) {
    /*
     * Byte 0: 0x0A
     * Byte 1: Character Index
     */

    UARTBuffer[0] = 0x0A;
    UARTBuffer[1] = characterIndex;

    while(!acknowledged) {}
    acknowledged = false;
    UART_transmit(2, UARTBuffer);
    UART_waitForAcknowledge();
}

void UART_setBackgroundColors(uint8_t stage) {
    /*
     * Byte 0: 0x0C
     * Byte 1: Stage Index
     */

    UARTBuffer[0] = 0x0C;
    UARTBuffer[1] = stage;

    while(!acknowledged) {}
    acknowledged = false;
    UART_transmit(2, UARTBuffer);
    UART_waitForAcknowledge();
}

void UART_readPersistentSprite(uint8_t spriteIndex, uint16_t x, uint16_t y) {
    /*
     * Byte 0: 0x0D
     * Byte 1: Sprite Index
     * Byte 2: (x >> 8) & 0xFF
     * Byte 3: (x) & 0xFF;
     * Byte 4: (y) & 0xFF
     */

    UARTBuffer[0] = 0x0D;
    UARTBuffer[1] = spriteIndex;
    UARTBuffer[2] = (x >> 8) & 0xFF;
    UARTBuffer[3] = x & 0xFF;
    UARTBuffer[4] = y;

    while(!acknowledged) {}
    acknowledged = false;
    UART_transmit(5, UARTBuffer);
    UART_waitForAcknowledge();
}
