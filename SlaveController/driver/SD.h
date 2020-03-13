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


extern void SD_init(void);

extern void SD_openFile(char* filename);
extern void SD_readFile(uint32_t numBytes, char* output);
extern void SD_closeFile(void);

extern void SD_startSDCard(void);
extern void SD_releaseSDCard(void);

#ifdef __cplusplus
}
#endif
