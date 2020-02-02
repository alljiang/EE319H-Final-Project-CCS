
/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/drivers/SDSPI.h>

/* Example/Board Header files */
#include "Board.h"

#include "ILI9341.h"
#include "SD_SPI.h"
#include "UART.h"

#define TASKSTACKSIZE   768

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

Void taskFxn(UArg arg0, UArg arg1)
{
//    SDSPI_startSDCard();
//
//    char filename[] = "test.txt";
//    SDSPI_openFile(filename);

    ILI9341_initGeneral();
    ILI9341_fillScreen(0);

    uint32_t rgb[] = {0xFFFFFF, 0xFF0000, 0x00FF00, 0x0000FF, 0xF0F000};
    uint32_t num[] = {15, 30, 15, 30, 10};

    for(int i = 5; i < 100; i++) {
        ILI9341_drawHLineMulticolored(5, i, rgb, num, 5);
    }

//    for(int y = 0; y < 42; y++) {
//        ILI9341_drawHLine(0, y, 160, 0b111111000000000000); //R
//        ILI9341_drawHLine(0, y, 160, 0b000000111111000000); //G
//        ILI9341_drawHLine(0, y, 160, 0b000000000000111111); //B
//        for(int c = 0; c < 160; c++) {
//            ILI9341_drawPixel(r,c,0b111111000000000000); //R
//            ILI9341_drawPixel(r,c,0b000000111111000000); //G
//            ILI9341_drawPixel(r,c,0b000000000000111111); //B
//        }
//    }
//    for(int y = 42; y < 87; y++) {
//        ILI9341_drawHLine(0, y, 160, 0b111111000000000000); //R
//        ILI9341_drawHLine(0, y, 160, 0b000000111111000000); //G
//        ILI9341_drawHLine(0, y, 160, 0b000000000000111111); //B
//        for(int c = 0; c < 160; c++) {
//            ILI9341_drawPixel(r,c,0b111111000000000000); //R
//            ILI9341_drawPixel(r,c,0b000000111111000000); //G
//            ILI9341_drawPixel(r,c,0b000000000000111111); //B
//        }
//    }
//    for(int y = 87; y < 128; y++) {
//        ILI9341_drawHLine(0, y, 160, 0b111111000000000000); //R
//        ILI9341_drawHLine(0, y, 160, 0b000000111111000000); //G
//        ILI9341_drawHLine(0, y, 160, 0b000000000000111111); //B
//        for(int c = 0; c < 160; c++) {
//            ILI9341_drawPixel(r,c,0b111111000000000000); //R
//            ILI9341_drawPixel(r,c,0b000000111111000000); //G
//            ILI9341_drawPixel(r,c,0b000000000000111111); //B
//        }
//    }
}

Int main()
{
    System_printf("Starting!\n");
    System_flush();

    Board_initGeneral();
    Board_initGPIO();
    Board_initSPI();
    Board_initUART();
    Board_initSDSPI();

    UART_start();
    SDSPI_initGeneral();

    System_printf("Board initialized\n");
    System_flush();

    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.priority = 1;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)taskFxn, &taskParams, NULL);

    BIOS_start();    /* does not return */

    return(0);
}
