
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
#include "Utils.h"

#define TASKSTACKSIZE   768

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

Void taskFxn(UArg arg0, UArg arg1)
{
//    SDSPI_startSDCard();
//
//    char filename[] = "test.txt";
//    SDSPI_openFile(filename);

    ILI9341_init();
    uint32_t t1 = millis();

    beginSPITransaction();

    uint32_t rgb[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFF0000, 0x00FF00};
    uint32_t num[] = {30, 120, 60, 30, 80};

    uint32_t rgb2[] = {0xFF00FF, 0xF000B2, 0xF0C000, 0x2F00F0, 0xF0F0F0};
    uint32_t num2[] = {120, 60, 30, 80, 30};

    for(int j = 0; j < 10; j++) {
        for(int i = 0; i <= 240; i++) {
            ILI9341_drawHLineMulticolored(0, i, rgb, num, 5);
        }
        for(int i = 0; i <= 240; i++) {
            ILI9341_drawHLineMulticolored(0, i, rgb2, num2, 5);
        }
    }

    endSPITransaction();

    uint32_t t2 = millis();

    millis();
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
