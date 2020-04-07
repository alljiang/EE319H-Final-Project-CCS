/*
 *  ILI9341 Driver for TM4C123GXL - Header file
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 */

#include <cstdint>



#define ILI9341_NOP           0x00
#define ILI9341_SWRESET       0x01
#define ILI9341_RDDID         0x04
#define ILI9341_RDDST         0x09

#define ILI9341_SLPIN         0x10
#define ILI9341_SLPOUT        0x11
#define ILI9341_PTLON         0x12
#define ILI9341_NORON         0x13

#define ILI9341_RDMODE        0x0A
#define ILI9341_RDMADCTL      0x0B
#define ILI9341_RDPIXFMT      0x0C
#define ILI9341_RDIMGFMT      0x0D
#define ILI9341_RDSELFDIAG    0x0F

#define ILI9341_INVOFF        0x20
#define ILI9341_INVON         0x21
#define ILI9341_GAMMASET      0x26
#define ILI9341_DISPOFF       0x28
#define ILI9341_DISPON        0x29

#define ILI9341_CASET         0x2A
#define ILI9341_PASET         0x2B
#define ILI9341_RAMWR         0x2C
#define ILI9341_RAMRD         0x2E

#define ILI9341_PTLAR         0x30
#define ILI9341_MADCTL        0x36
#define ILI9341_VSCRSADD      0x37
#define ILI9341_PIXFMT        0x3A

#define ILI9341_FRMCTR1       0xB1
#define ILI9341_FRMCTR2       0xB2
#define ILI9341_FRMCTR3       0xB3
#define ILI9341_INVCTR        0xB4
#define ILI9341_DISCTRL       0xB6

#define ILI9341_PWCTRL1       0xC0
#define ILI9341_PWCTRL2       0xC1
#define ILI9341_PWCTRL3       0xC2
#define ILI9341_PWCTRL4       0xC3
#define ILI9341_PWCTRL5       0xC4
#define ILI9341_VMCTRL1       0xC5
#define ILI9341_VMCTRL2       0xC7

#define ILI9341_RDID1         0xDA
#define ILI9341_RDID2         0xDB
#define ILI9341_RDID3         0xDC
#define ILI9341_RDID4         0xDD

#define ILI9341_GMCTRP1       0xE0
#define ILI9341_GMCTRN1       0xE1

#define ILI9341_TFTWIDTH      321
#define ILI9341_TFTHEIGHT     241
#define ST7735_TFTWIDTH       160
#define ST7735_TFTHEIGHT      128

/*
 *  @brief      Initializes the LCD screen
 */
extern void ILI9341_init(void);

/*
 *  @brief      Sets individual pixel on LCD
 *
 *  @param      x       x-coordinate of pixel
 *  @param      y       y-coordinate of pixel
 *  @param      rgb     6-6-6 RGB value of pixel
 */
extern void ILI9341_drawPixel(uint32_t x, uint32_t y, uint32_t rgb);

/*
 *  @brief      Sets individual pixel on LCD
 *
 *  @param      x       x-coordinate of left-most pixel
 *  @param      y       y-coordinate of line
 *  @param      l       length of line in pixels
 *  @param      rgb     6-6-6 RGB value of pixel
 */
extern void ILI9341_drawHLine(uint32_t x, uint32_t y, uint32_t l, uint32_t rgb);

/*
 *  @brief      Sets individual pixel on LCD
 *
 *  @param      x       x-coordinate of line
 *  @param      y       y-coordinate of bottom-most pixel
 *  @param      l       length of line in pixels
 *  @param      rgb     6-6-6 RGB value of pixel
 */
extern void ILI9341_drawVLine(uint32_t x, uint32_t y, uint32_t l, uint32_t rgb);

/*
 *  @brief       Draws a multi-colored line, optimized for efficiency
 *
 *  @param      x       x-coordinate of left-most pixel
 *  @param      y       y-coordinate of line
 *  @param      *rgb    pointer to array of RGB values (parallel to num, must be same array length)
 *  @param      *num    pointer to array of number of pixels corresponding to each RGB color
 *  @param      n       size of array
 */
extern void ILI9341_drawHLineMulticolored(uint32_t x, uint32_t y, uint32_t *rgb, uint32_t *num, uint32_t n);

extern void ILI9341_drawHLineMulticolored_indexed(uint32_t x, uint32_t y, uint16_t *rgb, uint16_t *num, uint32_t n);
extern void ILI9341_drawColors_indexed(uint32_t x, uint32_t y, int16_t *rgbIndex, uint16_t totalPixels, uint32_t n);
extern void ILI9341_drawColors(uint32_t x, uint32_t y, int32_t *rgbArr, uint16_t totalPixels);

/*
 *  @brief      Draws a single-color rectangle
 *
 *  @param      x       x-coordinate of bottom-left pixel
 *  @param      y       y-coordinate of bottom-left pixel
 *  @param      w       width of rectangle
 *  @param      h       height of rectangle
 *  @param      rgb     RGB color
 */
extern void ILI9341_fillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t rgb);

/*
 * @brief       Fills entire screen with one color
 *
 * @param       rgb     RGB value
 */
extern void ILI9341_fillScreen(uint32_t rgb);

/*
 *  @brief      Sends coordinate information to LCD
 *
 *  @param      x0      x-coordinate of top left pixel
 *  @param      y0      y-coordinate of top left pixel
 *  @param      x1      x-coordinate of bottom right pixel
 *  @param      y1      y-coordinate of bottom right pixel
 */
//extern void ILI9341_setCoords(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/*
 *  @brief      Sends color information to LCD
 *
 *  @param      rgb     6-6-6 rgb color
 */
//extern void ILI9341_setColor(uint32_t rgb);

/*
 *  @brief      Command, resets the software on the LCD
 */
extern void ILI9341_softwareReset(void);

/*
 *  @brief      Command, turns display on and off
 *
 *  @param      enable    Enables or disables display
 */
extern void ILI9341_enableDisplay(bool enable);

extern void commandList(const uint8_t *addr);

