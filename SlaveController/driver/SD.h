/*
 *  SD Card Driver - Header file
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void SD_openFile(char* filename);
extern void SD_read(uint32_t numBytes, uint8_t* buffer);
extern void SD_closeFile(void);
extern char SD_readNextChar();

extern void SD_startSDCard(void);
extern void SD_releaseSDCard(void);

#ifdef __cplusplus
}
#endif
