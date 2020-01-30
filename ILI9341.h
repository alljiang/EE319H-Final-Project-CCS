/*
 *  ILI9341 Driver for TM4C123GXL - Header file
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 */

#ifdef __cplusplus
extern "C" {
#endif


#define ILI9341_NOP         0x00
#define ILI9341_SWRESET     0x01
#define ILI9341_RDDID       0x04
#define ILI9341_RDDST       0x09

#define ILI9341_SLPIN       0x10
#define ILI9341_SLPOUT      0x11
#define ILI9341_PTLON       0x12
#define ILI9341_NORON       0x13

#define ILI9341_RDMODE      0x0A
#define ILI9341_RDMADCTL    0x0B
#define ILI9341_RDPIXFMT    0x0C
#define ILI9341_RDIMGFMT    0x0D
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF      0x20
#define ILI9341_INVON       0x21
#define ILI9341_GAMMASET    0x26
#define ILI9341_DISPOFF     0x28
#define ILI9341_DISPON      0x29

#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_RAMRD       0x2E

#define ILI9341_PTLAR       0x30
#define ILI9341_MADCTL      0x36
#define ILI9341_VSCRSADD    0x37
#define ILI9341_PIXFMT      0x3A

#define ILI9341_FRMCTR1     0xB1
#define ILI9341_FRMCTR2     0xB2
#define ILI9341_FRMCTR3     0xB3
#define ILI9341_INVCTR      0xB4
#define ILI9341_DFUNCTR     0xB6

#define ILI9341_PWCTR1      0xC0
#define ILI9341_PWCTR2      0xC1
#define ILI9341_PWCTR3      0xC2
#define ILI9341_PWCTR4      0xC3
#define ILI9341_PWCTR5      0xC4
#define ILI9341_VMCTR1      0xC5
#define ILI9341_VMCTR2      0xC7

#define ILI9341_RDID1       0xDA
#define ILI9341_RDID2       0xDB
#define ILI9341_RDID3       0xDC
#define ILI9341_RDID4       0xDD

#define ILI9341_GMCTRP1     0xE0
#define ILI9341_GMCTRN1     0xE1

#define ILI9341_TFTWIDTH    320
#define ILI9341_TFTHEIGHT   240

/*
 *  @brief      Initializes the LCD screen
 *  LIST ALL THE STUFF IT DOES HERE
 */
extern void ILI9341_initGeneral(void);

/*
 *  @brief      Sets individual pixel on LCD
 *
 *  @param      x       x-coordinate of pixel
 *  @param      y       y-coordinate of pixel
 *  @param      rgb     6-6-6 RGB value of pixel
 */
extern void ILI9341_setPixel(uint32_t x, uint32_t y, uint32_t rgb);

/*
 *  @brief      Sends coordinate information to LCD
 *
 *  @param      x0      x-coordinate of top left pixel
 *  @param      y0      y-coordinate of top left pixel
 *  @param      x1      x-coordinate of bottom right pixel
 *  @param      y1      y-coordinate of bottom right pixel
 */
extern void ILI9341_setCoords(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/*
 *  @brief      Sends color information to LCD
 *
 *  @param      rgb     6-6-6 rgb color
 */
extern void ILI9341_setColor(uint32_t rgb);

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

extern void writeCommand(uint8_t c);
extern void setCommandPin(bool isCommand);
extern void setResetPin(bool reset);
extern void chipSelect(bool select);
extern void beginSPITransaction(void);
extern void endSPITransaction(void);
extern void transferSPI(void);
extern void writeCommand(uint8_t c);
extern void writeData(uint8_t d);
extern void writeData16(uint16_t d);
extern void delay(uint32_t ms);

#ifdef __cplusplus
}
#endif
