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

#include "tm4c123gh6pm.h"
#include "ILI9341.h"
#include "Utils.h"
#include "Board.h"

#define TFT_CS                  (*((volatile uint32_t *)0x40004020))
#define TFT_CS_LOW              0           // CS normally controlled by hardware
#define TFT_CS_HIGH             0x08
#define DC                      (*((volatile uint32_t *)0x40004100))
#define DC_COMMAND              0
#define DC_DATA                 0x40
#define RESET                   (*((volatile uint32_t *)0x40004200))
#define RESET_LOW               0
#define RESET_HIGH              0x80

#define DELAY 0x80

const uint8_t
  cmd_st7735[] = {                 // Init for 7735R, part 1 (red or green tab)
    21,                       // 15 commands in list:
    ILI9341_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ILI9341_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ILI9341_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ILI9341_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ILI9341_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ILI9341_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ILI9341_PWCTRL1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ILI9341_PWCTRL2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ILI9341_PWCTRL3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ILI9341_PWCTRL4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,
    ILI9341_PWCTRL5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ILI9341_VMCTRL1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ILI9341_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ILI9341_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ILI9341_PIXFMT , 1      ,  // 15: set color mode, 1 arg, no delay:
    0b00000101,
    ILI9341_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ILI9341_PASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F,                        //  4 commands in list:
    ILI9341_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ILI9341_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ILI9341_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ILI9341_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                 //     16-bit color

static const uint8_t init_commands[] = {
    20,
    0xEF, 3, 0x03, 0x80, 0x02,
    0xCF, 3, 0x00, 0XC1, 0X30,
    0xED, 4, 0x64, 0x03, 0X12, 0X81,
    0xE8, 3, 0x85, 0x00, 0x78,
    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xF7, 1, 0x20,
    0xEA, 2, 0x00, 0x00,
    ILI9341_SWRESET,   DELAY, 150, // Software reset
    ILI9341_PWCTRL1, 1, 0b000111, //0x23, // Power control
    ILI9341_PWCTRL2, 1, 0x10, // Power control
    ILI9341_VMCTRL1, 2, 0b0010100, 0x28, //0x3e, 0x28, // VCM control
    ILI9341_VMCTRL2, 1, 0x86, // VCM control2
    ILI9341_MADCTL, 1, 0x48, // Memory Access Control
    ILI9341_PIXFMT, 1, 0b01010101,
    ILI9341_INVOFF, 0,
    ILI9341_DISCTRL, 5, 0x08, 0x82, 0x27, // Display Function Control
        0xF2, 0x00, // Gamma Function Disable
    ILI9341_GAMMASET, 1, 0x01, // Gamma curve selected
    ILI9341_GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
        0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
    ILI9341_GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
        0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
    ILI9341_FRMCTR1, 2, 0x00, 0x10, // FrameRate Control 119Hz
    ILI9341_NORON, DELAY, 10,  // Normal display on, 10ms delay
    ILI9341_DISPON, DELAY, 100
};

void DNU(void) {

    //  Initial LCD configuration
    beginSPITransaction();
    /*
    writeCommand(ILI9341_SWRESET);  // software reset
    delay(130); // wait 120ms

    writeCommand(ILI9341_SLPOUT);
    delay(120);
//    */
}

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
    uint8_t r = (rgb & 0xF80000) >> 19;
    uint8_t g = (rgb & 0x00FC00) >> 10;
    uint8_t b = (rgb & 0x0000F8) >> 3;

    writeData((g << 5) | (b));
    writeData((r << 3) | (g >> 3));
}

void static ILI9341_setCoords(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    x0 = ILI9341_TFTWIDTH - x0;
    x1 = ILI9341_TFTWIDTH - x1;
    y0 = ILI9341_TFTHEIGHT - y0;
    y1 = ILI9341_TFTHEIGHT - y1;

//    x0 = ST7735_TFTWIDTH - x0;
//    x1 = ST7735_TFTWIDTH - x1;
//    y0 = ST7735_TFTHEIGHT - y0;
//    y1 = ST7735_TFTHEIGHT - y1;

    writeCommand(ILI9341_CASET); // Column addr set
    writeData(y1>>8);
    writeData(y1);     // YSTART
    writeData(y0>>8);
    writeData(y0);     // YEND

    writeCommand(ILI9341_PASET); // Row addr set
    writeData(x1>>8);
    writeData(x1);     // XSTART
    writeData(x0>>8);
    writeData(x0);     // XEND

    writeCommand(ILI9341_RAMWR);    // write to RAM
}

static void deselect(void) {
    // wait until SSI0 not busy/transmit FIFO empty
  while((SSI0_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
  TFT_CS = TFT_CS_HIGH;
}

void ILI9341_init() {
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
    RESET = RESET_HIGH;
    delay(500);
    RESET = RESET_LOW;
    delay(500);
    RESET = RESET_HIGH;
    delay(500);

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
    SSI0_CR0_R &= ~(SSI_CR0_SCR_M |       // SCR = 0 (8 Mbps data rate)
                  SSI_CR0_SPH |         // SPH = 0
                  SSI_CR0_SPO);         // SPO = 0
                                        // FRF = Freescale format
    SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
                                        // DSS = 8-bit data
    SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_8;
    SSI0_CR1_R |= SSI_CR1_SSE;            // enable SSI

    commandList(cmd_st7735);

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
    if((x >= ILI9341_TFTWIDTH) || (y >= ILI9341_TFTHEIGHT)) return;

    beginSPITransaction();

    ILI9341_setCoords(x,y,x,y);

    ILI9341_setColor(rgb);

    deselect();
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
    deselect();
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
    deselect();
    endSPITransaction();
}

void ILI9341_drawHLineMulticolored(uint32_t x, uint32_t y, uint32_t *rgb, uint32_t *num, uint32_t n) {
    if((x > ILI9341_TFTWIDTH) || (y > ILI9341_TFTHEIGHT)) return;

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
    ILI9341_fillRect(0, 0, ILI9341_TFTWIDTH-1, ILI9341_TFTHEIGHT-1, rgb);
//    ILI9341_fillRect(0, 0, ST7735_TFTHEIGHT, ST7735_TFTWIDTH, rgb);
}

void beginSPITransaction(void) {
    SYSCTL_RCGCSSI_R |= 0x01;  // activate SSI0
}

void endSPITransaction(void) {
    while((SSI0_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
    SYSCTL_RCGCSSI_R &= ~0x01;  // deactivate SSI0
}
