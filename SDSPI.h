/*
 *  ILI9341 Driver for TM4C123GXL - Header file
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  January 2020
 */

#ifdef __cplusplus
extern "C" {
#endif


/*
 *  @brief      Initializes the LCD screen
 *  LIST ALL THE STUFF IT DOES HERE
 */
extern void ILI9341_initGeneral(void);


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

extern void setReadPin(bool isReadCommand);
extern void startSPI(void);
extern void releaseSPI(void);
extern void transferSPI(void);

#ifdef __cplusplus
}
#endif
