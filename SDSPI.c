/*
 *  ILI9341 Driver for TM4C123GXL
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
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDSPI.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "SDSPI.h"
#include "Board.h"

/* Buffer size used for the file copy process */
#ifndef CPY_BUFF_SIZE
#define CPY_BUFF_SIZE       2048
#endif

/* Drive number used for FatFs */
#define DRIVE_NUM           0

SDSPI_Handle sdspiHandle;
SDSPI_Params sdspiParams;

void SDSPI_initGeneral(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    chipSelect(false);

    startSDCard();
}

void readFile(char[] filename) {
    src = fopen("fat:"STR(DRIVE_NUM)":kirby.txt")
}

// PF2
// Sets CS pin of SD Card, MIGHT BE FLIPPED!
void chipSelect(bool select) {
    if(select) {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
    }
    else {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);
    }
}

void startSDCard(void) {
    SDSPI_Params_init(&sdspiParams);
    sdspiHandle = SDSPI_open(Board_SDSPI0, DRIVE_NUM, &sdspiParams);
    if (sdspiHandle == NULL) { System_abort("Error starting the SD card\n"); }
    else { System_printf("Drive %u is mounted\n", DRIVE_NUM); }
}

void releaseSDCard(void) {
    SPI_close(sdspiHandle);
}
