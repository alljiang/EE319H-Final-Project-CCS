/*
 *  SD Driver for TM4C123GXL
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SD_SPI.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDSPI.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "Board.h"
#include "SD_SPI.h"

/* Buffer size used for the file copy process */
#ifndef CPY_BUFF_SIZE
#define CPY_BUFF_SIZE       2048
#endif

SDSPI_Handle sdspiHandle;
SDSPI_Params sdspiParams;

FILE *src;
char fileHeader[] = "fat:0:";

void SDSPI_initGeneral(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    chipSelectSD(false);

    startSDCard();
}

void openFile(char* filename) {
    chipSelectSD(true);
    strcat(filename, fileHeader);
    src = fopen(filename, "r");
    if(!src) {
        System_printf("File not found");
        System_flush();
    }
    chipSelectSD(false);
}

void readFile(char* output, uint32_t numBytes) {
    chipSelectSD(true);
    uint32_t bytesRead;
    while(true) {
        bytesRead = fread(output, 1, numBytes, src);
        if(bytesRead == 0) {
            System_printf("Read Error or End of File");
            System_flush();
            break;
        }
    }
    chipSelectSD(false);
}

void closeFile(void) {
    chipSelectSD(true);
    fclose(src);
    chipSelectSD(false);
}

// PF2
// Sets CS pin of SD Card, MIGHT BE FLIPPED!
void chipSelectSD(bool select) {
    if(select) {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    }
    else {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    }
}

void startSDCard(void) {
    chipSelectSD(true);
    SDSPI_Params_init(&sdspiParams);
    sdspiHandle = SDSPI_open(Board_SDSPI0, 0, &sdspiParams);
    if (sdspiHandle == NULL) { System_abort("Error starting the SD card\n"); }
    else { System_printf("SD Card is mounted\n"); }
    chipSelectSD(false);
}

void releaseSDCard(void) {
    SDSPI_close(sdspiHandle);
}
