/*
 *  SD Card Driver for TM4C123GXL - Header file
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 */

#ifdef __cplusplus
extern "C" {
#endif


extern void SDSPI_initGeneral(void);

extern void openFile(char* filename);
extern void readFile(char* output, uint32_t numBytes);
extern void closeFile(void);

extern void chipSelectSD(bool select);
extern void startSDCard(void);
extern void releaseSDCard(void);

#ifdef __cplusplus
}
#endif
