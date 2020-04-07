/*
 *  SD Driver
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 */

/* XDC module Headers */
#include "Board.h"
#include <xdc/std.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDSPI.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/* Buffer size used for the file copy process */
#ifndef CPY_BUFF_SIZE
#define CPY_BUFF_SIZE       2048
#endif

SDSPI_Handle sdspiHandle;
SDSPI_Params sdspiParams;

FILE *src;

void SD_openFile(char filename[]) {
    char systemFilename[75] = "fat:0:";
    strcat(systemFilename, filename);

    src = fopen(systemFilename, "r");

    if(!src) {
        System_printf("File not found\n");
        System_flush();
    }
}

void SD_read(uint32_t numBytes, uint8_t* buffer) {
    if(fread(buffer, numBytes, 1, src) == 0) {
        System_printf("Read Error or End of File\n");
        System_flush();
    }
}

char SD_readNextChar() {
    char c;
    if(fread(&c, 1, 1, src) == 0) {
        System_printf("Read Error or End of File\n");
        System_flush();
    }
    return c;
}

void SD_closeFile(void) {
    fclose(src);
}

void SD_startSDCard(void) {
    SDSPI_Params_init(&sdspiParams);
    sdspiHandle = SDSPI_open(Board_SDSPI0, 0, &sdspiParams);
    if (sdspiHandle == NULL) { System_abort("Error starting the SD card\n"); }
    else { System_printf("SD Card is mounted\n"); }
    System_flush();
}

void SD_releaseSDCard(void) {
    SDSPI_close(sdspiHandle);
}
