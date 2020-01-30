/*
 *  ILI9341 Driver for TM4C123GXL
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 */

#include <stdlib.h>

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
unsigned char rxBuffer[100];

const uint32_t SPI_Bitrate = 40000000;  // 40 MHz

void ILI9341_initGeneral(void) {
    SPI_Params_init(&params);
//    params.bitRate = SPI_Bitrate;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_7 | GPIO_PIN_6);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //  Reset using RST pin

    setResetPin(true);
    delay(500);
    setResetPin(false);
    delay(500);
    setResetPin(true);
    delay(500);

    //  Initial LCD configuration
    beginSPITransaction();

    writeCommand(ILI9341_RDSELFDIAG);

/*
    writeCommand(ILI9341_SWRESET);  // software reset
    delay(50);

    writeCommand(ILI9341_SLPOUT);   // turn off sleep mode
    delay(500);

    writeCommand(ILI9341_PIXFMT);   // set pixel format to 18-bit
    writeData(0b01100110);
    delay(10);

    /* idek what these do
    writeCommand(0xEF);
    writeData(0x03);
    writeData(0x80);
    writeData(0x02);

    writeCommand(0xCF);
    writeData(0x00);
    writeData(0xC1);
    writeData(0x30);

    writeCommand(0xED);
    writeData(0x64);
    writeData(0x03);
    writeData(0x12);
    writeData(0x81);

    writeCommand(0xE8);
    writeData(0x85);
    writeData(0x00);
    writeData(0x78);

    writeCommand(0xCB);
    writeData(0x39);
    writeData(0x2C);
    writeData(0x00);
    writeData(0x34);
    writeData(0x02);

    writeCommand(0xF7);
    writeData(0x20);

    writeCommand(0xEA);
    writeData(0x00);
    writeData(0x00);

    /*

    writeCommand(ILI9341_PWCTR1);   // power control
    writeData(0x23);

    writeCommand(ILI9341_PWCTR2);   // power control
    writeData(0x10);

    writeCommand(ILI9341_VMCTR1);   // vcm control
    writeData(0x3e);
    writeData(0x28);

    writeCommand(ILI9341_VMCTR2);   // vcm control 2
    writeData(0x86);

    writeCommand(ILI9341_MADCTL);   // memory access control
    writeData(0x48);

    writeCommand(ILI9341_FRMCTR1);  // frame control
    writeData(0x00);
    writeData(0x18);

    writeCommand(ILI9341_DFUNCTR);  // display function control
    writeData(0x08);
    writeData(0x82);
    writeData(0x27);

    writeCommand(0xF2); // gamma function disable
    writeData(0x01);

    writeCommand(ILI9341_GAMMASET); // gamma curve selected
    writeData(0x01);

    writeCommand(ILI9341_GMCTRP1);  // set gamma
    writeData(0x0F);
    writeData(0x31);
    writeData(0x2B);
    writeData(0x0C);
    writeData(0x0E);
    writeData(0x08);
    writeData(0x4E);
    writeData(0xF1);
    writeData(0x37);
    writeData(0x07);
    writeData(0x10);
    writeData(0x03);
    writeData(0x0E);
    writeData(0x09);
    writeData(0x00);

    writeCommand(ILI9341_GMCTRN1);  // set gamma
    writeData(0x00);
    writeData(0x0E);
    writeData(0x14);
    writeData(0x03);
    writeData(0x11);
    writeData(0x07);
    writeData(0x31);
    writeData(0xC1);
    writeData(0x48);
    writeData(0x08);
    writeData(0x0F);
    writeData(0x0C);
    writeData(0x31);
    writeData(0x36);
    writeData(0x0F);

    writeCommand(ILI9341_FRMCTR1);  // framerate control 119hz
    writeData(0x10);

    writeCommand(ILI9341_SLPOUT);
    delay(120);
    */

    endSPITransaction();

    ILI9341_enableDisplay(true);
}

//  Bottom-left corner is (0, 0). Up = +y, Right = +x
void ILI9341_setPixel(uint32_t x, uint32_t y, uint32_t rgb) {
    if((x >= ILI9341_TFTWIDTH) || (y >= ILI9341_TFTHEIGHT)) return;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x,y);

    ILI9341_setColor(rgb);

    endSPITransaction();
}

void ILI9341_setColor(uint32_t rgb) {
    writeCommand(ILI9341_RAMWR);    // write to RAM

    writeData8((uint8_t) (rgb >> 16));
    writeData8((uint8_t) (rgb >> 8));
    writeData8((uint8_t) rgb);
}

void ILI9341_setCoords(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ILI9341_CASET); // Column addr set
    writeData(x0);     // XSTART
    writeData(x1);     // XEND

    writeCommand(ILI9341_PASET); // Row addr set
    writeData(y0);     // YSTART
    writeData(y1);     // YEND
}

void ILI9341_enableDisplay(bool enable) {
    beginSPITransaction();

    if(enable) { writeCommand(ILI9341_DISPON); }
    else { writeCommand(ILI9341_DISPOFF); }

    endSPITransaction();
}

// PA7  - D/C of board 1
// PC5  - D/C of board 2
// sets D/C pin     false: data (high)     true: command (low)
void setCommandPin(bool isCommand) {
    if(isCommand) {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
//        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);
    }
    else {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);
//        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_PIN_5);
    }
}

//  PA6 - RST pin of board 1 & 2
//  false: no reset (low)   true: reset (high)
void setResetPin(bool reset) {
    if(reset) {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_PIN_6);
    }
    else {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 0);
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

void writeCommand(uint8_t c) {
    setCommandPin(true);
    transaction.count = 1;

    uint8_t txBuffer[1];
    txBuffer[0] = c;

    transaction.txBuf = (Ptr) txBuffer;
    transaction.rxBuf = (Ptr) rxBuffer;
    transferSPI();
}

void writeData8(uint8_t d) {
    setCommandPin(false);
    transaction.count = 1;

    uint8_t txBuffer[1];
    txBuffer[0] = d;

    transaction.txBuf = (Ptr) txBuffer;
    transaction.rxBuf = (Ptr) rxBuffer;
    transferSPI();
}

void writeData16(uint16_t d) {
    setCommandPin(false);
    transaction.count = 2;

    uint8_t txBuffer[2];
    txBuffer[0] = d >> 8;
    txBuffer[1] = d & 0xFF;

    transaction.txBuf = (Ptr) txBuffer;
    transaction.rxBuf = (Ptr) rxBuffer;
    transferSPI();
}

void writeData(uint8_t d) {
    writeData8(d);
}

void beginSPITransaction(void) {
    spi = SPI_open(Board_SPI0, NULL);
    if (spi == NULL) { System_abort("Error initializing SPI\n"); }
    chipSelect(true);
}

void endSPITransaction(void) {
    chipSelect(false);
    SPI_close(spi);
}

// transfers SPI stuff using the global SPI handle and SPI transaction
void transferSPI(void) {
    bool transferOK = SPI_transfer(spi, &transaction);
    if(!transferOK) { System_printf("SPI Transfer Failed"); }
    System_flush();
}

void delay(uint32_t ms) {
    ms *= 6666;    // convert to system cycles
    for(; ms > 0; ms--) {}
}
