
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

#include "driver/ILI9341.h"
#include "driver/UART.h"
#include "driver/Utils.h"
#include "driver/SD.h"
#include "driver/SRAM.h"
#include "driver/Audio.h"

#define TASKSTACKSIZE 8000

Task_Struct taskStruct;

Char taskStack[TASKSTACKSIZE];

void startFxn(UArg arg0, UArg arg1)
{
    /*
    ILI9341_init();

    while(1) {
        ILI9341_fillScreen(0);
        Task_sleep(1000);
        ILI9341_fillScreen(0xFFFFFFF);
        Task_sleep(1000);
    }
    */

//    /*
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
//    */

    /*
    SRAM_init();

    uint8_t buffy[100];

    SRAM_readSFDP(buffy);


    memset(buffy, 0x55, 100);


//    buffy[0] = 0xAB;
//    buffy[1] = 0xCD;
//    buffy[25] = 0x12;
//    buffy[27] = 0x34;
//    buffy[49] = 0xEF;

//    SRAM_write(0, 50, buffy);

//    memset(buffy, 0x88, 100);
//    SRAM_read(0, 50, buffy);

//    memset(buffy, 0x88, 100);
//    SRAM_read(0, 50, buffy);
     */
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

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

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
