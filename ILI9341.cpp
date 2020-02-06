/*
 *  ILI9341 Driver for TM4C123GXL
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 *
 *      ST7735
 *   LED-: GND
 *   LED+: 3.3V
 *  SD_CS:
 *   MOSI:
 *   MISO:
 *    SCK: PA2, SD clock
 *     CS: PA3
 *    SCL: PA2
 *    SDA: PA5
 *     A0: PA6
 *  RESET: PA7
 *     NC: not connected
 *     NC: not connected
 *     NC: not connected
 *    VCC: 3.3V
 *    GND: GND
 *
 *
 *    ILI9341
 *    VCC: 3.3V
 *    GND: GND
 *     CS: PA3
 *  RESET: PA7
 *     DC: PA6
 *   MOSI: PA5
 *    SCK: PA2
 *    LED: 3.3V
 *   MISO: -
 *  T_CLK: -
 *   T_CS: -
 *  T_DIN: -
 *   T_DO: -
 *  T_IRQ: --

 */

#include <stdlib.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/SPI.h>
#include <ti/sysbios/knl/Semaphore.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "Utils.h"
#include "Board.h"
#include "colors.h"
#include "ILI9341.h"

#define DELAY 0x80

SPI_Handle spi;
SPI_Params params;
SPI_Transaction transaction;

uint8_t txBuffer[1000];
uint8_t rxBuffer[100];

Semaphore_Struct semStruct;
Semaphore_Handle semHandle;

bool cmdPinHigh;

static const uint8_t cmd_ili9341[] = {
    25,

    ILI9341_SWRESET, DELAY, 150, // Software reset

    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xCF, 3, 0x00, 0xC1, 0x30,
    0xEF, 3, 0x03, 0x80, 0x02,
    0xE8, 3, 0x85, 0x00, 0x78,
    0xEA, 2, 0x00, 0x00,
    0xCF, 3, 0x00, 0XC1, 0X30,
    0xED, 4, 0x64, 0x03, 0x12, 0x81,
    0xF7, 1, 0x20,

    ILI9341_PWCTRL1, 1, 0x23, // Power control
    ILI9341_PWCTRL2, 1, 0x10, // Power control
    ILI9341_VMCTRL1, 2, 0x3E, 0x28, //0x3e, 0x28, // VCM control
    ILI9341_VMCTRL2, 1, 0x86, // VCM control2
    ILI9341_MADCTL, 1, 0x48, // Memory Access Control
    ILI9341_PIXFMT, 1, 0x55, //16-bit    0x66 24-bit
    ILI9341_INVOFF, 0,
    ILI9341_FRMCTR1, 2, 0x00, 0x10, // FrameRate Control 119Hz
    ILI9341_DISCTRL, 4, 0x08, 0x82, 0x27, 0x00, // Display Function Control
    0x30, 4, 0x00, 0x00, 0x01, 0x3F,
    0xF2, 1, 0x00,
    ILI9341_GAMMASET, 1, 0x01, // Gamma curve selected
    ILI9341_GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
        0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
    ILI9341_GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
        0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
    ILI9341_SLPOUT, DELAY, 200,  // Normal display on, 10ms delay
    ILI9341_DISPON, DELAY, 100
};

void setCommandPin(bool isCommand) {
    if(isCommand) {
        if(cmdPinHigh) {
            Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
            Semaphore_post(semHandle);
        }
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 0);
        cmdPinHigh = false;
    }
    else {
        if(!cmdPinHigh) {
            Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
            Semaphore_post(semHandle);
        }
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_PIN_6);
        cmdPinHigh = true;
    }
}

void transferSPI() {
    Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
    SPI_transfer(spi, &transaction);
}

static void writeCommand(uint8_t c) {
    setCommandPin(true);
    transaction.count = 1;

    txBuffer[0] = c;

    transaction.txBuf = (Ptr)txBuffer;
    transaction.rxBuf = (Ptr)rxBuffer;
    transferSPI();
}

static void writeData(uint8_t d) {
    setCommandPin(false);
    transaction.count = 1;

    txBuffer[0] = d;

    transaction.txBuf = (Ptr)txBuffer;
    transaction.rxBuf = (Ptr)rxBuffer;

    transferSPI();
}

void static ILI9341_setColor(uint32_t rgb) {
      // 16-bit, 5-6-5
    uint8_t r = (rgb & 0xF80000) >> 19;
    uint8_t g = (rgb & 0x00FC00) >> 10;
    uint8_t b = (rgb & 0x0000F8) >> 3;

    setCommandPin(false);
    transaction.count = 2;

    txBuffer[0] = (r << 3) | (g >> 3);
    txBuffer[1] = (g << 5) | (b);
    transaction.txBuf = (Ptr)txBuffer;
    transferSPI();

}

void static ILI9341_setCoords(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
//    x0 = ILI9341_TFTWIDTH - x0;
//    x1 = ILI9341_TFTWIDTH - x1;
//    y0 = ILI9341_TFTHEIGHT - y0;
//    y1 = ILI9341_TFTHEIGHT - y1;

//    x0 = ST7735_TFTWIDTH - x0;
//    x1 = ST7735_TFTWIDTH - x1;
//    y0 = ST7735_TFTHEIGHT - y0;
//    y1 = ST7735_TFTHEIGHT - y1;

    writeCommand(ILI9341_CASET); // Column addr set

    setCommandPin(false);
    transaction.count = 4;

    txBuffer[0] = y0>>8;
    txBuffer[1] = y0;
    txBuffer[2] = y1 >> 8;
    txBuffer[3] = y1;
    transaction.txBuf = (Ptr)txBuffer;
    transferSPI();

    writeCommand(ILI9341_PASET); // Row addr set

    setCommandPin(false);
    transaction.count = 4;

    txBuffer[0] = x0>>8;
    txBuffer[1] = x0;
    txBuffer[2] = x1 >> 8;
    txBuffer[3] = x1;
    transaction.txBuf = (Ptr)txBuffer;
    transferSPI();

    writeCommand(ILI9341_RAMWR);    // write to RAM
}


void chipSelect(bool select) {
    if(select) {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
    }
    else {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
    }
}

void setResetPin(bool reset) {
    if(reset) {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);
    }
    else {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
    }
}

void SPICallback(SPI_Handle handle, SPI_Transaction * transaction) {
    Semaphore_post(semHandle);
}

// init
//------------------------------------------------------------------
void ILI9341_init() {

    SPI_Params_init(&params);
    params.bitRate = 2000000;
    params.transferMode = SPI_MODE_CALLBACK;
    params.transferCallbackFxn = &SPICallback;

    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semStruct, 1, &semParams);
    semHandle = Semaphore_handle(&semStruct);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7 | GPIO_PIN_6);

//  Reset using RST pin
    setResetPin(true);
    delay(5);
    setResetPin(false);
    delay(20);
    setResetPin(true);
    delay(150);

    //  Initial LCD configuration
    beginSPITransaction();

    commandList(cmd_ili9341);

    ILI9341_fillScreen(0);                 // set screen to black
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in ROM byte array.
void commandList(const uint8_t *addr) {

  uint8_t numCommands, numArgs;
  uint16_t ms;

  numCommands = *(addr++);               // Number of commands to follow
  while(numCommands--) {                 // For each command...
      writeCommand(*(addr++));             //   Read, issue command
    numArgs  = *(addr++);                //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
        writeData(*(addr++));              //     Read, issue argument
    }

    if(ms) {
      ms = *(addr++);             // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay(ms);
    }
  }
}

//  Bottom-left corner is (0, 0). Up = +y, Right = +x
void ILI9341_drawPixel(uint32_t x, uint32_t y, uint32_t rgb) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    ILI9341_setCoords(x,y,x,y);

    ILI9341_setColor(rgb);
}

//  Coordinate is left-most pixel of line
void ILI9341_drawHLine(uint32_t x, uint32_t y, uint32_t l, uint32_t rgb) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;
    if(x + l > ILI9341_TFTWIDTH) l = ILI9341_TFTWIDTH-x;

    ILI9341_setCoords(x,y,x+l,y);

    uint32_t i;
    for(i = 0; i < l; i++) {
        ILI9341_setColor(rgb);
    }
}

//  Coordinate is bottom pixel of line
void ILI9341_drawVLine(uint32_t x, uint32_t y, uint32_t l, uint32_t rgb) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;
    if(y + l > ILI9341_TFTHEIGHT) l = ILI9341_TFTHEIGHT-y;

    ILI9341_setCoords(x,y,x,y+l);

    int i;
    for(i = 0; i < l; i++) {
        ILI9341_setColor(rgb);
    }
}

//  Coordinate is left-most pixel of line
void ILI9341_drawHLineMulticolored(uint32_t x, uint32_t y, uint32_t *rgb, uint32_t *num, uint32_t n) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    int i;
    uint32_t l = 0;
    for(i = 0; i < n; i++) {
        l += num[i];
    }

    if(x + l > ILI9341_TFTWIDTH) {
        l = ILI9341_TFTWIDTH-x;
    }

    ILI9341_setCoords(x,y,x+l,y);

    uint32_t j;
    uint32_t loops;
    uint16_t index = 0;
    for(i = n-1; i >= 0; i--) {
        loops = num[i];
        for(j = 0; j < loops && l-- > 0; j++) {
            uint32_t rgbVal = rgb[i];
            uint8_t r = (rgbVal & 0xF80000) >> 16;
            uint8_t g = (rgbVal & 0x00FC00) >> 10;
            uint8_t b = (rgbVal & 0x0000F8) >> 3;

            txBuffer[index++] = (r) | (g >> 3);
            txBuffer[index++] = (g << 5) | (b);
        }
    }
    setCommandPin(false);
    transaction.count = index;
    transaction.txBuf = (Ptr)txBuffer;
    transferSPI();
}

/*
 *  Uses indexed colors form colors.h
 */
void ILI9341_drawHLineMulticolored_indexed(uint32_t x, uint32_t y, uint16_t *rgb, uint16_t *num, uint32_t n) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    int i;
    uint16_t l = 0;
    uint16_t startOffset = 0;
    for(i = 0; i < n; i++) {
        l += num[i];
    }

    if(x + l > ILI9341_TFTWIDTH) l = ILI9341_TFTWIDTH-x;

    //  trim beginning
    if(colors[rgb[n-1]] == (uint32_t) -1) {
        n--;
        l -= num[n];
    }

    //  trim end
    if(colors[rgb[0]] == (uint32_t) -1) {
        startOffset = 1;
    }


    if(startOffset == 1) {
        ILI9341_setCoords(x+num[0],y,x+l,y);
        l -= num[0];
    }
    else {
        ILI9341_setCoords(x,y,x+l,y);
    }

    uint16_t j;
    uint16_t loops;
    uint16_t index = 0;
    for(i = startOffset; i < n; i++) {
        loops = num[i];
        for(j = 0; j < loops && l-- > 0; j++) {
            uint32_t actualcolor = colors[rgb[i]];
            if(actualcolor != (uint32_t)-1) {
                uint8_t r = (actualcolor & 0xF80000) >> 16;
                uint8_t g = (actualcolor & 0x00FC00) >> 10;
                uint8_t b = (actualcolor & 0x0000F8) >> 3;

                txBuffer[index++] = (r) | (g >> 3);
                txBuffer[index++] = (g << 5) | (b);
            }
            else {
                txBuffer[index++] = 0;
                txBuffer[index++] = 0;
            }
        }
    }
}

//  coordinate is bottom-left of rectangle
void ILI9341_fillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t rgb) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    if(x + w >= ILI9341_TFTWIDTH) w = ILI9341_TFTWIDTH - x;
    if(y + h >= ILI9341_TFTHEIGHT) h = ILI9341_TFTHEIGHT - y;

    ILI9341_setCoords(x, y, x+w, y+h);

    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            ILI9341_setColor(rgb);
        }
    }
}

void ILI9341_fillScreen(uint32_t rgb) {
    ILI9341_fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, rgb);
}

void beginSPITransaction() {
    spi = SPI_open(Board_SPI0, &params);
    if (spi == NULL) { System_abort("Error initializing SPI\n"); }
}

void endSPITransaction() {
    SPI_close(spi);
}

