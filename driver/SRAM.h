/*
 *  SRAM Driver for IS25LP080D
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

#include <stdint.h>
#include <ti/drivers/SPI.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IS25LP080D_NORD                 0x03   //  Normal Read Mode
#define IS25LP080D_FRD                  0x0B   //  Fast Read Mode
#define IS25LP080D_FRDIO                0xBB   //  Fast Read Dual I/O
#define IS25LP080D_FRDO                 0x3B   //  Fast Read Dual Output
#define IS25LP080D_FRQIO                0xEB   //  Fast Read Quad I/O
#define IS25LP080D_FRQO                 0x6B   //  Fast Read Quad Output
#define IS25LP080D_FRDTR                0x0D   //  Fast Read DTR Mode
#define IS25LP080D_FRDDTR               0xBD   //  Fast Read Dual I/O DTR
#define IS25LP080D_FRQDTR               0xED   //  Fast Read Quad I/O DTR
#define IS25LP080D_PP                   0x02   //  Input Page Program
#define IS25LP080D_PPQ_1                0x32   //  Quad Input Page Program
#define IS25LP080D_PPQ_2                0x38   //  Quad Input Page Program
#define IS25LP080D_SER                  0xD7   //  Sector Erase
#define IS25LP080D_BER32                0x52   //  Block Erase 32Kbyte
#define IS25LP080D_BER64                0xD8   //  Block Erase 64Kbyte
#define IS25LP080D_CER                  0xC7   //  Chip Erase
#define IS25LP080D_WREN                 0x06   //  Write Enable
#define IS25LP080D_WRDI                 0x04   //  Write Disable
#define IS25LP080D_RDSR                 0x05   //  Read Status Register
#define IS25LP080D_WRSR                 0x01   //  Write Status Register
#define IS25LP080D_RDFR                 0x48   //  Read Function Register
#define IS25LP080D_WRFR                 0x42   //  Write Function Register
#define IS25LP080D_QPIEN                0x35   //  Enter QPI mode
#define IS25LP080D_QPIDI                0xF5   //  QPI Exit, QPI only command
#define IS25LP080D_PERSUS               0x75   //  Suspend during program/erase
#define IS25LP080D_PERRSM               0x7A   //  Resume program/erase
#define IS25LP080D_DP                   0xB9   //  Deep Power Down
#define IS25LP080D_RDID                 0xAB   //  Read ID
#define IS25LP080D_RDPD                 0xAB   //  Release Power Down
#define IS25LP080D_SRPNV                0x65   //  Set Read Parameters (Non-Volatile)
#define IS25LP080D_SRPV                 0xC0   //  Set Read Parameters (Volatile)
#define IS25LP080D_SERPNV               0x85   //  Set Extended Read Parameters (Non-Volatile)
#define IS25LP080D_SERPV                0x83   //  Set Extended Read Parameters (Volatile)
#define IS25LP080D_RDRP                 0x61   //  Read Read Parameters (Volatile)
#define IS25LP080D_RDERP                0x81   //  Read Extended Read Parameters (Volatile)
#define IS25LP080D_CLERP                0x82   //  Clear Extended Read Register
#define IS25LP080D_RDJDID               0x9F   //  Read JEDEC ID Command
#define IS25LP080D_RDMDID               0x90   //  Read Manufacturer & Device ID
#define IS25LP080D_RDJDIDQ              0xAF   //  Read JEDEC ID QPI mode
#define IS25LP080D_RDUID                0x4B   //  Read Unique ID
#define IS25LP080D_RDSFDP               0x5A   //  SFDP Read
#define IS25LP080D_NOP                  0x00   //  No Operation
#define IS25LP080D_RSTEN                0x66   //  Software Reset Enable
#define IS25LP080D_RST                  0x99   //  Software Reset
#define IS25LP080D_IRER                 0x64   //  Erase Information Row
#define IS25LP080D_IRP                  0x62   //  Program Information Row
#define IS25LP080D_IRRD                 0x68   //  Read Information Row
#define IS25LP080D_SECUNLOCK            0x26   //  Sector Unlock
#define IS25LP080D_SECLOCK              0x24   //  Sector Lock
#define IS25LP080D_RDABR                0x14   //  Read AutoBoot Register
#define IS25LP080D_WRABR                0x15   //  Write AutoBoot Register

extern void SRAM_init();
extern void SRAM_read(uint32_t address, uint32_t numBytes, uint8_t* buffer);
extern void SRAM_write(uint32_t address, uint32_t numBytes, uint8_t* buffer);
extern void SRAM_transferSPI();
extern void SRAM_transferSPICS(bool setCS);
extern void SRAM_writeCommand(uint8_t cmd);
extern void SRAM_writeCommandCS(uint8_t cmd, bool setCS);


#ifdef __cplusplus
}
#endif
