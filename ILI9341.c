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

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "ILI9341.h"
#include "Board.h"

SPI_Handle spi;
SPI_Params params;
SPI_Transaction transaction;

const uint32_t SPI_Bitrate = 10000000;  // 10 MHz

void ILI9341_initGeneral(void) {
    SPI_Params_init(&params);
    params.bitRate = SPI_Bitrate;

    startSPI();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    chipSelect(false);
}

void ILI9341_softwareReset(void) {

}

void ILI9341_enableDisplay(bool enable) {
    chipSelect(true);
    setReadPin(false);
    uint16_t txBuffer[1];

    if(enable) { txBuffer[0] = 0x28; }
    else { txBuffer[0] = 0x29; }

    transaction.count = 1;
    transaction.txBuf = txBuffer;

    transferSPI();
    setReadPin(true);
    chipSelect(false);
}

// PA7  - D/C of board 1
// PC5  - D/C of board 2
// sets D/C pin     0/false: command  1/true: read
void setReadPin(bool isReadCommand) {
    if(isReadCommand) {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_PIN_5);
    }
    else {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);
    }
}

// PA3  - CS of board 1
// PC4  - CS of board 2
// Sets CS pin of both LCDs
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

void startSPI(void) {
    spi = SPI_open(Board_SPI0, &params);
    if (spi == NULL) { System_abort("Error initializing SPI\n"); }
}

void releaseSPI(void) {
    SPI_close(spi);
}

// transfers SPI stuff using the global SPI handle and SPI transaction
void transferSPI(void) {
    bool transferOK = SPI_transfer(spi, &transaction);
    if(!transferOK) { System_printf("SPI Transfer Failed"); }
}
