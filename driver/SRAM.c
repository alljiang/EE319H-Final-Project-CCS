/*
 *  SRAM Driver for IS25LP080D
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

/* XDC module Headers */
#include <driver/Board.h>
#include <xdc/std.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "SRAM.h"

SPI_Handle spi;
SPI_Params params;
SPI_Transaction transaction;

uint8_t txBufferGlobal[1];

Semaphore_Struct semStruct;
Semaphore_Handle semHandle;
bool transferComplete = true;

void SPICallback(SPI_Handle handle, SPI_Transaction * transaction) {
    Semaphore_post(semHandle);
    transferComplete = true;
}

void SRAM_init() {

    SPI_Params_init(&params);
    params.bitRate = 100000;
    params.transferMode = SPI_MODE_CALLBACK;
    params.transferCallbackFxn = &SPICallback;
    spi = SPI_open(EK_TM4C123GXL_SPI0, &params);

    if (spi == NULL) {
        System_abort("Error initializing SPI\n");
        System_flush();
    }


    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semStruct, 1, &semParams);
    semHandle = Semaphore_handle(&semStruct);

    writeCommand(IS25LP080D_WREN);   // Write enable
    writeCommand(IS25LP080D_CER);    // Erases entire memory array
    writeCommand(IS25LP080D_WRDI);   // Write disable
}

void SRAM_read(uint32_t address, uint32_t numBytes, uint8_t* buffer) {
    uint8_t txBuffer[4];
    transaction.count = numBytes + 4;
    txBuffer[0] = IS25LP080D_NORD;
    txBuffer[1] = (address & 0x0F0000) >> 16;
    txBuffer[2] = (address & 0x00FF00) >> 8;
    txBuffer[3] = (address & 0x0000FF);

    transaction.txBuf = (Ptr)txBuffer;
    transaction.rxBuf = (Ptr)(buffer);    // Make sure to throw out first 4 bytes of data!

    transferSPI();
}

void SRAM_write(uint32_t address, uint32_t numBytes, uint8_t* buffer) {
    writeCommand(IS25LP080D_WREN);   // Write enable

    uint8_t txBuffer[300];
    uint16_t i;

    while(numBytes > 256) {
        transaction.count = 256 + 4;
        txBuffer[0] = IS25LP080D_PP;
        txBuffer[1] = (address & 0x0F0000) >> 16;
        txBuffer[2] = (address & 0x00FF00) >> 8;
        txBuffer[3] = (address & 0x0000FF);
        for(i = 0; i < 256; i++) {
            txBuffer[i+4] = buffer[i];
        }

        transaction.txBuf = (Ptr) txBuffer;
        transaction.rxBuf = NULL;

        transferSPI_blocking();

        numBytes -= 256;
        address += 256;
        buffer += 256;
    }

    transaction.count = numBytes;
    txBuffer[0] = IS25LP080D_PP;
    txBuffer[1] = (address & 0x0F0000) >> 16;
    txBuffer[2] = (address & 0x00FF00) >> 8;
    txBuffer[3] = (address & 0x0000FF);
    for(i = 0; i < numBytes; i++) {
        txBuffer[i+4] = buffer[i];
    }

    transaction.txBuf = (Ptr) txBuffer;
    transaction.rxBuf = NULL;

    transferSPI_blocking();

    writeCommand(IS25LP080D_WRDI);   // Write disable
}

void writeCommand(uint8_t cmd) {
    transaction.count = 1;
    txBufferGlobal[0] = cmd;
    transaction.txBuf = (Ptr) txBufferGlobal;
    transaction.rxBuf = NULL;

    transferSPI_blocking();
}

void transferSPI() {
    Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
    transferComplete = false;
    SPI_transfer(spi, &transaction);
}

void transferSPI_blocking() {
    Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
    transferComplete = false;
    SPI_transfer(spi, &transaction);
    while(!transferComplete) {}
}
