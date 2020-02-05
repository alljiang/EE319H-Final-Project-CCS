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

extern void SDSPI_openFile(char* filename);
extern void SDSPI_readFile(char* output, uint32_t numBytes);
extern void SDSPI_closeFile(void);

extern void SDSPI_startSDCard(void);
extern void SDSPI_releaseSDCard(void);

#ifdef __cplusplus
}
#endif
