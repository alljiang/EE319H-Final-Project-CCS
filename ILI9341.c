/*
 *  ILI9341 Driver for TM4C123GXL
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 *
 *      ST7735
 *   LED-: GND
 *   LED+: 3.3V
 *  SD_CS: PF2, SD CS
 *   MOSI: PA5, SD MOSI
 *   MISO: PA4, SD MISO
 *    SCK: PA2, SD clock
 *     CS: PA3
 *    SCL: PA2
 *    SDA: PA5
 *     A0: PA7
 *  RESET: PA6
 *     NC: not connected
 *     NC: not connected
 *     NC: not connected
 *    VCC: 3.3V
 *    GND: GND
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
#include "Utils.h"
#include "Board.h"

SPI_Handle spi;
SPI_Params params;
SPI_Transaction transaction;

uint8_t txBuffer[100];
uint8_t rxBuffer[100];

const uint32_t SPI_Bitrate = 20000000;  // 20 MHz
bool spiOpen = true;

void spiCallback(SPI_Handle handle,
                 SPI_Transaction * transaction) {
    chipSelect(false);
    spiOpen = true;
}

void ILI9341_initGeneral(void) {
    SPI_Params_init(&params);
    params.bitRate = SPI_Bitrate;
//    params.transferMode = SPI_MODE_CALLBACK;
//    params.transferCallbackFxn = &spiCallback;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_7 | GPIO_PIN_6);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

//  Reset using RST pin
    chipSelect(true);
    setResetPin(true);
    delay(1);   // minimum of 10us
    setResetPin(false);
    delay(1);   // minimum of 10us
    setResetPin(true);
    delay(5);   // wait 5 ms before sending commands, 120 for slpout
    chipSelect(false);

    //  Initial LCD configuration
    beginSPITransaction();

///*
    writeCommand(ILI9341_SWRESET);  // software reset
    delay(130); // wait 120ms

    writeCommand(ILI9341_SLPOUT);   // turn off sleep mode
    delay(130); // wait 120ms (ST7735)
//    delay(5);   // wait 5ms before sending next command, allow voltage to stabilize (ILI9341)

    writeCommand(ILI9341_FRMCTR1);
    writeData(0x01);
    writeData(0x2C);
    writeData(0x2D);

    writeCommand(ILI9341_FRMCTR2);
    writeData(0x01);
    writeData(0x2C);
    writeData(0x2D);

    writeCommand(ILI9341_FRMCTR3);
    writeData(0x01);
    writeData(0x2C);
    writeData(0x2D);
    writeData(0x01);
    writeData(0x2C);
    writeData(0x2D);

    writeCommand(ILI9341_INVCTR);
    writeData(0x07);

    writeCommand(ILI9341_PWCTRL1);   // power control
    writeData(0b11111);
    writeData(0x02);

    writeCommand(ILI9341_PWCTRL2);   // power control
    writeData(0xC5);

    writeCommand(ILI9341_PWCTRL3);
    writeData(0x0A);
    writeData(0x00);

    writeCommand(ILI9341_PWCTRL4);
    writeData(0x8A);
    writeData(0x2A);

    writeCommand(ILI9341_PWCTRL5);
    writeData(0x8A);
    writeData(0xEE);

    writeCommand(ILI9341_VMCTRL1);
    writeData(0x0E);

    writeCommand(ILI9341_INVOFF);

    writeCommand(ILI9341_MADCTL);
    writeData(0xC8);

    writeCommand(ILI9341_CASET);
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);
    writeData(0x7F);

    writeCommand(ILI9341_PASET);
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);
    writeData(0x9F);

    writeCommand(ILI9341_GMCTRP1);
    writeData(0x02);
    writeData(0x1C);
    writeData(0x07);
    writeData(0x12);
    writeData(0x37);
    writeData(0x32);
    writeData(0x29);
    writeData(0x2D);
    writeData(0x29);
    writeData(0x25);
    writeData(0x2B);
    writeData(0x39);
    writeData(0x00);
    writeData(0x01);
    writeData(0x03);
    writeData(0x10);

    writeCommand(ILI9341_GMCTRN1);
    writeData(0x03);
    writeData(0x1D);
    writeData(0x07);
    writeData(0x06);
    writeData(0x2E);
    writeData(0x2C);
    writeData(0x29);
    writeData(0x2D);
    writeData(0x2E);
    writeData(0x2E);
    writeData(0x37);
    writeData(0x3F);
    writeData(0x00);
    writeData(0x00);
    writeData(0x02);
    writeData(0x10);

    writeCommand(ILI9341_NORON);
    delay(10);

    writeCommand(ILI9341_DISPON);

    writeCommand(ILI9341_PIXFMT);   // set pixel format to 18-bit
    writeData(0b00000110); //18-bit

//*/
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
//*/
    /*
    writeCommand(ILI9341_SWRESET);  // software reset
    delay(130); // wait 120ms

    writeCommand(ILI9341_SLPOUT);
    delay(120);

    writeCommand(ILI9341_PWCTRL1);   // power control
    writeData(0b000111);

    writeCommand(ILI9341_PWCTRL2);   // power control
    writeData(0x10);

    writeCommand(ILI9341_VMCTRL1);   // vcm control
    writeData(0b0010100);
    writeData(0x28);

    writeCommand(ILI9341_VMCTRL2);   // vcm control 2
    writeData(0x86);

    writeCommand(ILI9341_MADCTL);   // memory access control
    writeData(0x48);

    writeCommand(ILI9341_FRMCTR1);  // frame control
    writeData(0x00);
    writeData(0x18);

    writeCommand(ILI9341_INVOFF);

    writeCommand(ILI9341_PIXFMT);   // set pixel format to 18-bit
    writeData(0b01100110);

    writeCommand(ILI9341_DISCTRL);  // display function control
    writeData(0x08);
    writeData(0x82);
    writeData(0x27);

//    writeCommand(0xF2); // gamma function disable
//    writeData(0x01);

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


//    */

    endSPITransaction();
}

//  Bottom-left corner is (0, 0). Up = +y, Right = +x
void ILI9341_drawPixel(uint32_t x, uint32_t y, uint32_t rgb) {
    if((x >= ILI9341_TFTWIDTH) || (y >= ILI9341_TFTHEIGHT)) return;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x,y);

    ILI9341_setColor(rgb);

    endSPITransaction();
}

//  Coordinate is left-most pixel of line
void ILI9341_drawHLine(uint32_t x, uint32_t y, uint32_t l, uint32_t rgb) {
    if((x >= ILI9341_TFTWIDTH) || (y >= ILI9341_TFTHEIGHT)) return;

    if(x + l >= ILI9341_TFTWIDTH) l = ILI9341_TFTWIDTH-x-1;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x+l,y);

    uint32_t i;
    for(i = 0; i < l; i++) {
        ILI9341_setColor(rgb);
    }

    endSPITransaction();
}

//  Coordinate is bottom pixel of line
void ILI9341_drawVLine(uint32_t x, uint32_t y, uint32_t l, uint32_t rgb) {
    if((x >= ILI9341_TFTWIDTH) || (y >= ILI9341_TFTHEIGHT)) return;

    if(y + l >= ILI9341_TFTHEIGHT) l = ILI9341_TFTHEIGHT-y-1;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x,y+l);


    int i;
    for(i = 0; i < l; i++) {
        ILI9341_setColor(rgb);
    }

    endSPITransaction();
}

void ILI9341_drawHLineMulticolored(uint32_t x, uint32_t y, uint32_t *rgb, uint32_t *num, uint32_t n) {
    if((x >= ILI9341_TFTWIDTH) || (y >= ILI9341_TFTHEIGHT)) return;

    int i;
    uint32_t l = 0;
    for(i = 0; i < n; i++) {
        l += num[i];
    }

    if(x + l >= ILI9341_TFTWIDTH) l = ILI9341_TFTWIDTH-x-1;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x+l,y);

    uint32_t j;
    uint32_t loops;
    for(i = n-1; i >= 0; i--) {
        loops = num[i];
        for(j = 0; j < loops && l-- > 0; j++) {
            ILI9341_setColor(rgb[i]);
        }
    }

    endSPITransaction();
}

//  coordinate is bottom-left of rectange
void ILI9341_fillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t rgb) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    if(x + w >= ILI9341_TFTWIDTH) w = ILI9341_TFTWIDTH - x;
    if(y + h >= ILI9341_TFTHEIGHT) h = ILI9341_TFTHEIGHT - y;

    beginSPITransaction();

    ILI9341_setCoords(x, y, x+w, y+h);

    uint32_t i;
    for(i = 0; i < w * h; i++) {
        ILI9341_setColor(rgb);
    }

    endSPITransaction();
}

void ILI9341_fillScreen(uint32_t rgb) {
//    ILI9341_fillRect(0, 0, ILI9341_TFTWIDTH-1, ILI9341_TFTHEIGHT-1, rgb);
    ILI9341_fillRect(0, 0, 160, 128, rgb);
}

void ILI9341_setColor(uint32_t rgb) {
    uint8_t r = (rgb & 0x3F0000) >> 16;
    uint8_t g = (rgb & 0x003F00) >> 8;
    uint8_t b = (rgb & 0x00003F);

    setCommandPin(false);
    transaction.count = 3;

    txBuffer[0] = b << 2;
    txBuffer[1] = g << 2;
    txBuffer[2] = r << 2;

    transaction.txBuf = (Ptr)txBuffer;
    transaction.rxBuf = (Ptr)rxBuffer;
    transferSPI();
}

void ILI9341_setCoords(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
//    x0 = ILI9341_TFTWIDTH - x0;
//    x1 = ILI9341_TFTWIDTH - x1;
//    y0 = ILI9341_TFTHEIGHT - y0;
//    y1 = ILI9341_TFTHEIGHT - y1;

    x0 = 160 - x0;
    x1 = 160 - x1;
    y0 = 128 - y0;
    y1 = 128 - y1;

    writeCommand(ILI9341_CASET); // Column addr set
    writeData(y1>>8);
    writeData8(y1, false);     // YSTART
    writeData8(y0>>8, false);
    writeData8(y0, false);     // YEND

    writeCommand(ILI9341_PASET); // Row addr set
    writeData(x1>>8);
    writeData8(x1, false);     // XSTART
    writeData8(x0>>8, false);
    writeData8(x0, false);     // XEND

    writeCommand(ILI9341_RAMWR);    // write to RAM
}

void ILI9341_enableDisplay(bool enable) {
    beginSPITransaction();

    if(enable) { writeCommand(ILI9341_DISPON); }
    else { writeCommand(ILI9341_DISPOFF); }

    endSPITransaction();
}

// PA7  - D/C of LCD
// sets D/C pin     false: data (high)     true: command (low)
void setCommandPin(bool isCommand) {
    if(isCommand) {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
    }
    else {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);
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

    txBuffer[0] = c;

    transaction.txBuf = (Ptr)txBuffer;
    transaction.rxBuf = (Ptr)rxBuffer;
    transferSPI();
}

void writeData8(uint8_t d, bool setDC) {
//    while(&0x00000002)==0){};   // wait until transmit FIFO not full
//    SSI0_DR_R = c;

    if(setDC) setCommandPin(false);

    transaction.count = 1;

    txBuffer[0] = d;

    transaction.txBuf = (Ptr)txBuffer;
    transaction.rxBuf = (Ptr)rxBuffer;

    transferSPI();
}

void writeData16(uint16_t d) {
    setCommandPin(false);
    transaction.count = 2;

    txBuffer[0] = d >> 8;
    txBuffer[1] = d & 0xFF;

    transaction.txBuf = (Ptr)txBuffer;
    transaction.rxBuf = (Ptr)rxBuffer;
    transferSPI();
}

void writeData(uint8_t d) {
    writeData8(d, true);
}

void beginSPITransaction(void) {
    spi = SPI_open(Board_SPI0, &params);
    if (spi == NULL) { System_abort("Error initializing SPI\n"); }
}

void endSPITransaction(void) {
    SPI_close(spi);
}

void readSPI(uint32_t numBytes) {
    transaction.count = numBytes;
    transaction.txBuf = NULL;
    transaction.rxBuf = (Ptr)rxBuffer;
    transferSPI();
}

// transfers SPI stuff using the global SPI handle and SPI transaction
void transferSPI() {
    while(!spiOpen) {}
    chipSelect(true);
//    spiOpen = false;
    bool transferOK = SPI_transfer(spi, &transaction);
    if(!transferOK) {
        System_printf("SPI Transfer Failed");
        System_flush();
    }
    chipSelect(false);
}
