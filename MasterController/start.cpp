
/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/drivers/SDSPI.h>
#include <ti/drivers/GPIO.h>

#include <string.h>

/* Example/Board Header files */
#include "driver/Board.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "driver/UART.h"
#include "driver/Utils.h"
#include "driver/SD.h"
#include "driver/Audio.h"

#define TASKSTACKSIZE 8000

Task_Struct taskStruct;

Char taskStack[TASKSTACKSIZE];

void startFxn(UArg arg0, UArg arg1)
{
    Audio_initSD();
    Audio_destroyAllAudio();

    AudioParams audioparams;
    Audio_initParams(&audioparams);

    audioparams.soundIndex = 2;
    audioparams.volume = 0.5;
    int8_t background = Audio_playAudio(audioparams);

    while(1) {
        ReadSDFIFO();
        sleep(1);
    }
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

    Audio_init();

    System_printf("Board initialized\n");
    System_flush();

    Task_Params params;
    Task_Params_init(&params);
    params.stackSize = TASKSTACKSIZE;

    params.priority = 1;
    params.stack = &taskStack;
    Task_construct(&taskStruct, (Task_FuncPtr)startFxn, &params, NULL);

    BIOS_start();    /* does not return */

    return(0);
}
