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

#include "Utils.h"
#include "Flash.h"

SPI_Handle Flash_spi;
SPI_Params Flash_spi_params;
SPI_Transaction Flash_transaction;

uint8_t Flash_txBuffer[10];
uint32_t currentLocation = 0;

void Flash_init() {

    SPI_Params_init(&Flash_spi_params);
    Flash_spi_params.bitRate = 20000000;
    Flash_spi = SPI_open(EK_TM4C123GXL_SPI3, &Flash_spi_params);

    if (Flash_spi == NULL) {
        System_abort("Error initializing SPI\n");
        System_flush();
    }

    //    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
    //    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);

    sleep(50);

    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);

    Flash_writeCommand(IS25LP080D_WREN);   // Write enable
    Flash_writeCommand(IS25LP080D_CER);    // Erases entire memory array
    Flash_pendWriteOpCompletion();
    Flash_writeCommand(IS25LP080D_WRDI);   // Write disable
}

uint8_t* Flash_readMemory(uint32_t address, uint32_t numBytes, uint8_t* buffer) {
    Flash_transaction.count = numBytes + 4;
    Flash_txBuffer[0] = IS25LP080D_NORD;
    Flash_txBuffer[1] = (address & 0xFF0000) >> 16;
    Flash_txBuffer[2] = (address & 0x00FF00) >> 8;
    Flash_txBuffer[3] = (address & 0x0000FF);

    Flash_transaction.txBuf = (Ptr)Flash_txBuffer;
    Flash_transaction.rxBuf = (Ptr)(buffer);    // Make sure to throw out first 4 bytes of data!

    Flash_transferSPI();

    return buffer + 4;
}

void Flash_readSFDP(uint8_t* buffer) {
    Flash_transaction.count = 9;
    Flash_txBuffer[0] = IS25LP080D_RDSFDP;
    Flash_txBuffer[1] = 0;
    Flash_txBuffer[2] = 0;
    Flash_txBuffer[3] = 0;
    Flash_txBuffer[4] = 0;

    Flash_transaction.txBuf = (Ptr)Flash_txBuffer;
    Flash_transaction.rxBuf = (Ptr)(buffer);    // Make sure to throw out first 4 bytes of data!

    Flash_transferSPI();
}

void Flash_pendWriteOpCompletion() {
    uint8_t receiveBuffer[2];
    bool completed = false;
    int num = 0;

    do {
        num++;
        Flash_transaction.count = 2;
        Flash_txBuffer[0] = IS25LP080D_RDSR;

        Flash_transaction.txBuf = (Ptr)Flash_txBuffer;
        Flash_transaction.rxBuf = (Ptr)(receiveBuffer);    // Make sure to throw out first 4 bytes of data!

        Flash_transferSPI();

        /*
         * p. 18 of datasheet
         * Bit 0 of RDSR is Write in Progress Bit (WIP).
         *      "0" indicates device is ready
         *      "1" indicates a write cycle is in progress and device is busy
         */
        completed = !(receiveBuffer[1] & 1);

    } while(!completed);
}

void Flash_writeMemory_specifiedAddress(uint32_t address, uint32_t numBytes, uint8_t* buffer) {
    while(numBytes > 0) {
        Flash_writeCommand(IS25LP080D_WREN);   // Write enable
        int16_t bytesAvailableThisPage = 256-(address % 256);

        uint16_t bytesThisPage;
        if(numBytes > bytesAvailableThisPage) bytesThisPage = bytesAvailableThisPage;
        else bytesThisPage = numBytes;

        Flash_transaction.count = bytesThisPage + 4;
        Flash_txBuffer[0] = IS25LP080D_PP;
        Flash_txBuffer[1] = (address & 0x0F0000) >> 16;
        Flash_txBuffer[2] = (address & 0x00FF00) >> 8;
        Flash_txBuffer[3] = (address & 0x0000FF);
        for(uint16_t i = 0; i < bytesThisPage; i++) {
            Flash_txBuffer[i+4] = buffer[i];
        }

        Flash_transaction.txBuf = (Ptr) Flash_txBuffer;
        Flash_transaction.rxBuf = NULL;

        Flash_transferSPI();

        numBytes -= bytesThisPage;
        address += bytesThisPage;
        buffer += bytesThisPage;
        Flash_pendWriteOpCompletion();

    }
    Flash_writeCommand(IS25LP080D_WRDI);   // Write disable
}

uint32_t Flash_writeMemory(uint32_t bytesToWrite, uint8_t* buffer) {
    uint32_t toReturn = currentLocation;
    Flash_writeMemory_specifiedAddress(currentLocation, bytesToWrite, buffer);
    currentLocation += bytesToWrite;
    return toReturn;
}

void Flash_writeCommandCS(uint8_t cmd, bool setCS) {
    Flash_txBuffer[0] = cmd;

    Flash_transaction.txBuf = (Ptr) Flash_txBuffer;
    Flash_transaction.rxBuf = NULL;
    Flash_transaction.count = 1;

    Flash_transferSPICS(setCS);
}

void Flash_writeCommand(uint8_t cmd) {
    Flash_writeCommandCS(cmd, true);
}

void Flash_transferSPICS(bool setCS) {
    if(setCS) {
//        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
    }
    SPI_transfer(Flash_spi, &Flash_transaction);
    if(setCS) {
//        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    }
}

void Flash_transferSPI() {
    Flash_transferSPICS(true);
}

/*
 *  Reserves space
 */
uint32_t Flash_allocateMemory(uint32_t bytesToAllocate) {
    currentLocation += bytesToAllocate;
    return currentLocation-bytesToAllocate;
}
