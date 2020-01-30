
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
    ILI9341_initGeneral();

    ILI9341_setPixel(50,50,0x31992);
}

Int main()
{
    System_printf("Starting!\n");
    System_flush();

    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();
    Board_initSPI();
    Board_initSDSPI();

    SDSPI_initGeneral();
    UART_start();

    System_printf("Board initialized\n");
    System_flush();

    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)taskFxn, &taskParams, NULL);

    BIOS_start();    /* does not return */

    return(0);
}
