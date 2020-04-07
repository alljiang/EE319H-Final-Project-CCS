/*
 *  ILI9341 Driver for TM4C123GXL
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 *
 *
 *              ILI9341
 *
 *                       T_IRQ: --
 *                        T_DO: -
 *                       T_DIN: -
 *  SD_SCK: PB4           T_CS: -
 * SD_MISO: PB6          T_CLK: -
 * SD_MOSI: PB7           MISO: -
 *   SD_CS: PB5            LED: 3.3V
 *                         SCK: PA2
 *                        MOSI: PA5
 *                          DC: PA6
 *                       RESET: PE5
 *                          CS: PA3
 *                         GND: GND
 *                         VCC: 3.3V

 */

#include <driver/Board.h>
#include <driver/ILI9341.h>
#include <driver/Utils.h>
#include <driver/v_tm4c123gh6pm.h>
#include <colors.h>
#include <stdlib.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>



#define TFT_CS                  (*((volatile uint32_t *)0x40004020))
#define TFT_CS_LOW              0           // CS normally controlled by hardware
#define TFT_CS_HIGH             0x08
#define DC                      (*((volatile uint32_t *)0x40004100))
#define DC_COMMAND              0
#define DC_DATA                 0x40
#define RESET                   (*((volatile uint32_t *)0x40024000))
#define RESET_LOW               0
#define RESET_HIGH              0x20

#define DELAY 0x80

void beginSPITransaction();
void endSPITransaction();

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

static void writeCommand(uint8_t c) {
                                        // wait until SSI0 not busy/transmit FIFO empty
  while((SSI0_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
  TFT_CS = TFT_CS_LOW;
  DC = DC_COMMAND;
  SSI0_DR_R = c;                        // data out
                                        // wait until SSI0 not busy/transmit FIFO empty
  while((SSI0_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
}

static void writeData(uint8_t d) {
  while((SSI0_SR_R&SSI_SR_TNF)==0){};   // wait until transmit FIFO not full
  DC = DC_DATA;
  SSI0_DR_R = d;                        // data out
}

void static ILI9341_setColor(uint32_t rgb) {
    /* // 18-bit, 6-6-6
    uint8_t r = (rgb & 0x3F0000) >> 16;
    uint8_t g = (rgb & 0x003F00) >> 8;
    uint8_t b = (rgb & 0x00003F);

    writeData(b << 2);
    writeData(g << 2);
    writeData(r << 2);

    */

      // 16-bit, 5-6-5
    //    uint8_t r = (rgb & 0xF80000) >> 19;
    //    uint8_t g = (rgb & 0x00FC00) >> 10;
    //    uint8_t b = (rgb & 0x0000F8) >> 3;
    uint8_t r = (rgb & 0xF80000) >> 19;
    uint8_t g = (rgb & 0x00FC00) >> 10;
    uint8_t b = (rgb & 0x0000F8) >> 3;

    writeData((r << 3) | (g >> 3));
    writeData((g << 5) | (b));
}

void static ILI9341_setCoords(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(ILI9341_CASET); // Column addr set
    writeData(y0>>8);
    writeData(y0);     // YSTART
    writeData(y1>>8);
    writeData(y1);     // YEND

    writeCommand(ILI9341_PASET); // Row addr set
    writeData(x0>>8);
    writeData(x0);     // XSTART
    writeData(x1>>8);
    writeData(x1);     // XEND

    writeCommand(ILI9341_RAMWR);    // write to RAM
}

static void deselect(void) {
    // wait until SSI0 not busy/transmit FIFO empty
  while((SSI0_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
  TFT_CS = TFT_CS_HIGH;
}

// init
//------------------------------------------------------------------
void ILI9341_init() {
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5);

    SYSCTL_RCGCSSI_R |= 0x01;  // activate SSI0
    SYSCTL_RCGCGPIO_R |= 0x01; // activate port A
    while((SYSCTL_PRGPIO_R&0x01)==0){}; // allow time for clock to start

    // toggle RST low to reset; CS low so it'll listen to us
    // SSI0Fss is temporarily used as GPIO
    GPIO_PORTA_DIR_R |= 0xC8;             // make PA3,6,7 out
    GPIO_PORTA_AFSEL_R &= ~0xC8;          // disable alt funct on PA3,6,7
    GPIO_PORTA_DEN_R |= 0xC8;             // enable digital I/O on PA3,6,7
                                        // configure PA3,6,7 as GPIO
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0x00FF0FFF)+0x00000000;
    GPIO_PORTA_AMSEL_R &= ~0xC8;          // disable analog functionality on PA3,6,7
    TFT_CS = TFT_CS_LOW;
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5);
    sleep(10);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0);
    sleep(20);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5);
    sleep(150);

    // initialize SSI0
    GPIO_PORTA_AFSEL_R |= 0x2C;           // enable alt funct on PA2,3,5
    GPIO_PORTA_DEN_R |= 0x2C;             // enable digital I/O on PA2,3,5
                                        // configure PA2,3,5 as SSI
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFF0F00FF)+0x00202200;
    GPIO_PORTA_AMSEL_R &= ~0x2C;          // disable analog functionality on PA2,3,5
    SSI0_CR1_R &= ~SSI_CR1_SSE;           // disable SSI
    SSI0_CR1_R &= ~SSI_CR1_MS;            // master mode
                                        // configure for system clock/PLL baud clock source
    SSI0_CC_R = (SSI0_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_SYSPLL;

                                        // SysClk/(CPSDVSR*(1+SCR))
                                        // 80/(10*(1+0)) = 8 MHz (slower than 4 MHz)
    SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+4; // must be even number
//    SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+6; // must be even number
    SSI0_CR0_R &= ~(SSI_CR0_SCR_M |       // SCR = 0 (8 Mbps data rate)
                  SSI_CR0_SPH |         // SPH = 0
                  SSI_CR0_SPO);         // SPO = 0
                                        // FRF = Freescale format
    SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
                                        // DSS = 8-bit data
    SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_8;
    SSI0_CR1_R |= SSI_CR1_SSE;            // enable SSI

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
      sleep(ms);
    }
  }
}

//  Bottom-left corner is (0, 0). Up = +y, Right = +x
void ILI9341_drawPixel(uint32_t x, uint32_t y, uint32_t rgb) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x,y);

    ILI9341_setColor(rgb);

    deselect();
    endSPITransaction();
}

//  Coordinate is left-most pixel of line
void ILI9341_drawHLine(uint32_t x, uint32_t y, uint32_t l, uint32_t rgb) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;
    if(x + l > ILI9341_TFTWIDTH) l = ILI9341_TFTWIDTH-x;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x+l,y);

    for(uint32_t i = 0; i < l; i++) {
        ILI9341_setColor(rgb);
    }
    deselect();
    endSPITransaction();
}

//  Coordinate is bottom pixel of line
void ILI9341_drawVLine(uint32_t x, uint32_t y, uint32_t l, uint32_t rgb) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;
    if(y + l > ILI9341_TFTHEIGHT) l = ILI9341_TFTHEIGHT-y;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x,y+l);

    for(uint16_t i = 0; i < l; i++) {
        ILI9341_setColor(rgb);
    }
    deselect();
    endSPITransaction();
}

void ILI9341_drawColors(uint32_t x, uint32_t y, int32_t *rgbArr, uint16_t totalPixels) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    if(x + totalPixels >= ILI9341_TFTWIDTH) totalPixels = ILI9341_TFTWIDTH-x;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x+totalPixels,y);

    for(uint32_t i = 0; i < totalPixels; i++) {
        ILI9341_setColor(rgbArr[i]);
    }

    deselect();
}

//  Coordinate is left-most pixel of line
void ILI9341_drawHLineMulticolored(uint32_t x, uint32_t y, uint32_t *rgb, uint32_t *num, uint32_t n) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    uint32_t l = 0;
    for(int i = 0; i < n; i++) {
        l += num[i];
    }

    if(x + l >= ILI9341_TFTWIDTH) l = ILI9341_TFTWIDTH-x-1;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x+l,y);

    uint32_t j;
    uint32_t loops;
    for(int i = n-1; i >= 0; i--) {
        loops = num[i];
        for(j = 0; j < loops && l-- > 0; j++) {
            ILI9341_setColor(rgb[i]);
        }
    }
    deselect();
}

/*
 *  Uses indexed colors form colors.h
 */
void ILI9341_drawHLineMulticolored_indexed(uint32_t x, uint32_t y, uint16_t *rgb, uint16_t *num, uint32_t n) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    uint16_t l = 0;
    uint16_t startOffset = 0;
    for(int i = 0; i < n; i++) {
        l += num[i];
    }

    if(x + l >= ILI9341_TFTWIDTH) l = ILI9341_TFTWIDTH-x;

    beginSPITransaction();

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

    for(int i = startOffset; i < n; i++) {
        uint16_t loops = num[i];

        for(uint16_t j = 0; j < loops && l-- > 0; j++) {
            uint32_t actualcolor = colors[rgb[i]];
            if(actualcolor != (uint32_t)-1) {
                ILI9341_setColor(actualcolor);
            }
            else {
                ILI9341_setColor(0);
//                ILI9341_setColor(0x00FF00);
            }
        }
    }
    deselect();
}

//  coordinate is bottom-left of rectangle
void ILI9341_fillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t rgb) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

    if(x + w >= ILI9341_TFTWIDTH) w = ILI9341_TFTWIDTH - x;
    if(y + h >= ILI9341_TFTHEIGHT) h = ILI9341_TFTHEIGHT - y;

    beginSPITransaction();

    ILI9341_setCoords(x, y, x+w, y+h);

    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            ILI9341_setColor(rgb);
        }
    }
    deselect();
}

void ILI9341_fillScreen(uint32_t rgb) {
    ILI9341_fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, rgb);
}

void beginSPITransaction(void) {
    SYSCTL_RCGCSSI_R |= 0x01;  // activate SSI0
}

void endSPITransaction(void) {
    while((SSI0_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
    SYSCTL_RCGCSSI_R &= ~0x01;  // deactivate SSI0
}
