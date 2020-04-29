/*
 *  UART Driver
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  March 2020
 */

/* XDCtools Header files */
#include <driver/Board.h>
#include <driver/UART.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include <stdint.h>
#include "Utils.h"

UART_Handle uart;
UART_Params uartParams;

uint8_t UARTBuffer[50];
uint8_t UARTReceiveBuffer[4];
volatile bool acknowledged;
volatile bool writeComplete;
bool skipNextUpdate;
int animationsUpdated;

void readCallbackFunction(UART_Handle handle, uint8_t *buf, size_t count) {
    acknowledged = true;
}

void writeCallbackFunction(UART_Handle handle, uint8_t *buf, size_t count) {
    writeComplete = true;
}

void UART_start(void) {
    UART_Params_init(&uartParams);
    uartParams.readMode = UART_MODE_CALLBACK;
    //    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.writeMode = UART_MODE_CALLBACK;
    uartParams.readCallback = &readCallbackFunction;
    uartParams.writeCallback = &writeCallbackFunction;

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 5000000;
    uart = UART_open(Board_UART1, &uartParams);

    //  slave will send an acknowledge byte on startup, master will only start after slave has started
    acknowledged = false;
    writeComplete = true;
    skipNextUpdate = false;
    animationsUpdated = 0;

    if (uart == NULL) {
        System_abort("Error opening UART");
    }
}

void UART_transmit(uint8_t numBytes, uint8_t *buffer) {
    while(!writeComplete) {}
    writeComplete = false;
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
    UARTBuffer[5] = (y >> 8) & 0xFF;
    UARTBuffer[6] = (y) & 0xFF;
    UARTBuffer[7] = frame;
    UARTBuffer[8] = persistent;
    UARTBuffer[9] = layer;
    UARTBuffer[10] = continuous;
    UARTBuffer[11] = framePeriod;
    UARTBuffer[12] = mirrored;

    if(skipNextUpdate || (animationsUpdated == 0 && !acknowledged)) {
        skipNextUpdate = true;
        return;
    }
    animationsUpdated++;
    while(!acknowledged) {}
    acknowledged = false;
    UART_transmit(13, UARTBuffer);
    UART_waitForAcknowledge();
}

void UART_commandUpdate() {
    /*
     * Byte 0: 0xFE
     */

    if(skipNextUpdate || animationsUpdated == 0) {
        skipNextUpdate = false;
        animationsUpdated = 0;
        return;
    }

    UARTBuffer[0] = 0xFE;

    while(!acknowledged) {}
    acknowledged = false;
    animationsUpdated = 0;
    skipNextUpdate = false;
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

void UART_commandReset() {
    /*
     * Byte 0: x0E
     */

    UARTBuffer[0] = 0x0E;

    while(!acknowledged) {}
    acknowledged = false;
    UART_transmit(1, UARTBuffer);
    UART_waitForAcknowledge();
}
