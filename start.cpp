
/* XDC module Headers */
#include <data/image.h>
#include <driver/SD.h>
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
#include "driver/Board.h"

#include "driver/ILI9341.h"
#include "driver/UART.h"
#include "driver/Utils.h"
#include "driver/SD.h"
#include "driver/SRAM.h"

#define TASKSTACKSIZE   768

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

Void taskFxn(UArg arg0, UArg arg1)
{
//    SD_startSDCard();
//
//    char filename[] = "kirby.txt";
//    SD_openFile(filename);
//
//    char buffer[100];
//    SD_readFile(buffer, 200);
//
//    System_printf(buffer);
//    System_flush();
//
//    SD_releaseSDCard();

//    ILI9341_init();
//    uint32_t t1 = millis();
//
//    beginSPITransaction();
//    ILI9341_fillScreen(0);
//
//    uint32_t currentY = 80;
//
//    uint32_t frame, row;
//    for(int i = 0; i < 1000; i++) {
//        for(frame = 0; frame <2 ; frame++) {
//            row = 0;
//            int startingRow = frame * 32;
//            currentY = 80;
//            for(row = startingRow; row < startingRow + 32; row++) {
//                uint16_t rgb[50];
//                uint16_t num[50];
//
//                int numColors = falling[row][0];
//
//                for(int n = 0; n < numColors; n++) {
//                    rgb[n] = falling[row][n*2+1];
//                    num[n] = falling[row][n*2+2];
//
//                }
//
//                ILI9341_drawHLineMulticolored_indexed(3, currentY--, rgb, num, numColors);
//            }
//            delay(100);
//        }
//    }

//    uint32_t rgb[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFF0000, 0x00FF00};
//    uint32_t num[] = {30, 120, 60, 30, 80};
//
//    uint32_t rgb2[] = {0xFF00FF, 0xF000B2, 0xF0C000, 0x2F00F0, 0xF0F0F0};
//    uint32_t num2[] = {120, 60, 30, 80, 30};
//
//    for(int j = 0; j < 10; j++) {
//        for(int i = 0; i <= 240; i++) {
//            ILI9341_drawHLineMulticolored(0, i, rgb, num, 5);
//        }
//        for(int i = 0; i <= 240; i++) {
//            ILI9341_drawHLineMulticolored(0, i, rgb2, num2, 5);
//        }
//    }

    SRAM_init();
    SRAM_read(0x11F0F, 10, NULL);

    uint8_t arr[530];
    arr[255] = 0xFE;
    arr[256] = 0xFF;
    arr[511] = 0xCE;
    arr[512] = 0xCF;
    arr[529] = 0xAA;

    SRAM_write(0x11F0F, 530, arr);

    uint32_t t2 = millis();
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
    SD_init();

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
