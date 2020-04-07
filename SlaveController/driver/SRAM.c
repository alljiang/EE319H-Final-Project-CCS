/*
 *  SRAM Driver for IS25LP080D
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 *
 *    SCK: PD0
 *     CS: PD1
 *   MISO: PD2
 *   MOSI: PD3
 *
 */

/* XDC module Headers */
#include <driver/Board.h>
#include <xdc/std.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <string.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/BIOS.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "SRAM.h"
#include "Utils.h"

SPI_Handle SRAM_spi;
SPI_Params SRAM_spi_params;
SPI_Transaction SRAM_transaction;

uint8_t SRAM_txBuffer[300];
uint32_t currentLocation = 0;

void SRAM_init() {

    SPI_Params_init(&SRAM_spi_params);
    SRAM_spi_params.bitRate = 20000000;
    SRAM_spi = SPI_open(EK_TM4C123GXL_SPI3, &SRAM_spi_params);

    if (SRAM_spi == NULL) {
        System_abort("Error initializing SPI\n");
        System_flush();
    }

    //    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
    //    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);

    sleep(50);

    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);

    SRAM_writeCommand(IS25LP080D_WREN);   // Write enable
    sleep(50);
    SRAM_writeCommand(IS25LP080D_CER);    // Erases entire memory array
    sleep(50);
    SRAM_writeCommand(IS25LP080D_WRDI);   // Write disable
    sleep(50);
}

uint8_t* SRAM_readMemory(uint32_t address, uint32_t numBytes, uint8_t* buffer) {
    SRAM_transaction.count = numBytes + 4;
    SRAM_txBuffer[0] = IS25LP080D_NORD;
    SRAM_txBuffer[1] = (address & 0xFF0000) >> 16;
    SRAM_txBuffer[2] = (address & 0x00FF00) >> 8;
    SRAM_txBuffer[3] = (address & 0x0000FF);

    SRAM_transaction.txBuf = (Ptr)SRAM_txBuffer;
    SRAM_transaction.rxBuf = (Ptr)(buffer);    // Make sure to throw out first 4 bytes of data!

    SRAM_transferSPI();

    return buffer + 4;
}

void SRAM_readSFDP(uint8_t* buffer) {
    SRAM_transaction.count = 9;
    SRAM_txBuffer[0] = IS25LP080D_RDSFDP;
    SRAM_txBuffer[1] = 0;
    SRAM_txBuffer[2] = 0;
    SRAM_txBuffer[3] = 0;
    SRAM_txBuffer[4] = 0;

    SRAM_transaction.txBuf = (Ptr)SRAM_txBuffer;
    SRAM_transaction.rxBuf = (Ptr)(buffer);    // Make sure to throw out first 4 bytes of data!

    SRAM_transferSPI();
}

void SRAM_writeMemory_specifiedAddress(uint32_t address, uint32_t numBytes, uint8_t* buffer) {
    SRAM_writeCommand(IS25LP080D_WREN);   // Write enable
    while(numBytes > 0) {
        int16_t bytesAvailableThisPage = 256-(address % 256);

        uint16_t bytesThisPage;
        if(numBytes > bytesAvailableThisPage) bytesThisPage = bytesAvailableThisPage;
        else bytesThisPage = numBytes;

        SRAM_transaction.count = bytesThisPage + 4;
        SRAM_txBuffer[0] = IS25LP080D_PP;
        SRAM_txBuffer[1] = (address & 0x0F0000) >> 16;
        SRAM_txBuffer[2] = (address & 0x00FF00) >> 8;
        SRAM_txBuffer[3] = (address & 0x0000FF);
        for(uint16_t i = 0; i < bytesThisPage; i++) {
            SRAM_txBuffer[i+4] = buffer[i];
        }

        SRAM_transaction.txBuf = (Ptr) SRAM_txBuffer;
        SRAM_transaction.rxBuf = NULL;

        SRAM_transferSPI();

        numBytes -= bytesThisPage;
        address += bytesThisPage;
        buffer += bytesThisPage;
//        sleep(1000);
    }
    SRAM_writeCommand(IS25LP080D_WRDI);   // Write disable
}

uint32_t SRAM_writeMemory(uint32_t bytesToWrite, uint8_t* buffer) {
    uint32_t toReturn = currentLocation;
    SRAM_writeMemory_specifiedAddress(currentLocation, bytesToWrite, buffer);
    currentLocation += bytesToWrite;
    return toReturn;
}

void SRAM_writeCommandCS(uint8_t cmd, bool setCS) {
    SRAM_txBuffer[0] = cmd;

    SRAM_transaction.txBuf = (Ptr) SRAM_txBuffer;
    SRAM_transaction.rxBuf = NULL;
    SRAM_transaction.count = 1;

    SRAM_transferSPICS(setCS);
}

void SRAM_writeCommand(uint8_t cmd) {
    SRAM_writeCommandCS(cmd, true);
}

void SRAM_transferSPICS(bool setCS) {
    if(setCS) {
//        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
    }
    SPI_transfer(SRAM_spi, &SRAM_transaction);
    if(setCS) {
//        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    }
}

void SRAM_transferSPI() {
    SRAM_transferSPICS(true);
}

/*
 *  Reserves space
 */
uint32_t SRAM_allocateMemory(uint32_t bytesToAllocate) {
    currentLocation += bytesToAllocate;
    return currentLocation-bytesToAllocate;
}