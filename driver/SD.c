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

void SD_init(void) {
    SDSPI_Params_init(&sdspiParams);
//    sdspiParams.bitRate = 12500000;
}

void SD_openFile(char filename[]) {
    char systemFilename[75] = "fat:0:";
    strcat(systemFilename, filename);

    src = fopen(systemFilename, "r");

    if(!src) {
        System_printf("File not found");
        System_flush();
    }
}

void SD_readFile(uint32_t numBytes, char* buffer) {
    uint32_t bytesRead;
    bytesRead = fread(buffer, numBytes, 1, src);
    if(bytesRead == 0) {
        System_printf("Read Error or End of File");
        System_flush();
    }
}

void SD_closeFile(void) {
    fclose(src);
}

void SD_startSDCard(void) {
    sdspiHandle = SDSPI_open(Board_SDSPI0, 0, &sdspiParams);
    if (sdspiHandle == NULL) { System_abort("Error starting the SD card\n"); }
    else { System_printf("SD Card is mounted\n"); }
    System_flush();
}

void SD_releaseSDCard(void) {
    SDSPI_close(sdspiHandle);
}
