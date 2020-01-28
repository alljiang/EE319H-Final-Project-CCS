/*
 *  ILI9341 Driver for TM4C123GXL
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>

#include "ILI9341.h"
#include "Board.h"

SPI_Handle spi;
SPI_Transaction transaction;

void ILI9341_initGeneral(void) {
    spi = SPI_open(Board_SPI0, NULL);
    if (spi == NULL) { System_abort("Error initializing SPI\n"); }
    else { System_printf("SPI initialized\n"); }
}

void ILI9341_softwareRest(void) {

}

void ILI9341_enableDisplay(bool enable) {
    uint16_t txBuffer[1];

    if(enable) { txBuffer[0] = 0x28; }
    else { txBuffer[0] = 0x29; }

    transaction.count = 1;
    transaction.txBuf = txBuffer;

    transferSPI();
}


// transfers SPI stuff using the global SPI handle and SPI transaction
void transferSPI(void) {
    bool transferOK = SPI_transfer(spi, &transaction);
    if(!transferOK) { System_printf("SPI Transfer Failed"); }
}
