
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

void taskFxn(UArg arg0, UArg arg1)
{
//    ILI9341_init();
//
//    while(1) {
//        ILI9341_fillScreen(0);
//        Task_sleep(1000);
//        ILI9341_fillScreen(0xFFFFFFF);
//        Task_sleep(1000);
//    }

//    Audio_initSD();
//    Audio_destroyAllAudio();
//
//    AudioParams audioparams;
//    Audio_initParams(&audioparams);
//
//    audioparams.soundIndex = 0;
//    audioparams.volume = 0.5;
//    int8_t background = Audio_playAudio(audioparams);
//
//    for(int i = 0; i < 10; i++) {
//        sleep(1000);
//        audioparams.soundIndex = 1;
//        audioparams.volume = 1;
//        Audio_playAudio(audioparams);
//    }
//
//    sleep(10000);
//    audioparams.soundIndex = 1;
//    audioparams.volume = 1;
//    int8_t countdown = Audio_playAudio(audioparams);
//
//    sleep(10000);
//    Audio_destroyAudio(background);
//
//    audioparams.soundIndex = 4;
//    audioparams.volume = 1;
//    int8_t beep = Audio_playAudio(audioparams);
//    sleep(3000);
//    Audio_destroyAudio(beep);
//
//    audioparams.soundIndex = 2;
//    audioparams.volume = 1;
//    int8_t song = Audio_playAudio(audioparams);
//
//    Task_sleep(500);
//    Audio_init();
//
//    while(1) {
//        ReadSDFIFO();
//        sleep(1);
//    }

    SRAM_init();

    uint8_t buffy[100];
//
//    memset(buffy, 0x55, 100);
//
//    buffy[0] = 0xAA;
//    SRAM_write(0, 1, buffy);
//    buffy[0] = 0;
//    SRAM_read(0, 1, buffy);
//
//    buffy[0] = 0xBB;
//    SRAM_write(0, 1, buffy);
//    buffy[0] = 0;
//    SRAM_read(0, 1, buffy);
//
//    buffy[0] = 0xCC;
//    SRAM_write(0, 1, buffy);
//    buffy[0] = 0;
//    SRAM_read(0, 1, buffy);

//    buffy[0] = 0xAB;
//    buffy[1] = 0xCD;
//    buffy[25] = 0x12;
//    buffy[27] = 0x34;
//    buffy[49] = 0xEF;
//
//    SRAM_write(0, 50, buffy);

    memset(buffy, 0x88, 100);

    Task_sleep(1);

    SRAM_read(0, 50, buffy);

    memset(buffy, 0x88, 100);

    SRAM_read(0, 50, buffy);
}

Int main()
{
    System_printf("Starting!\n");
    System_flush();

    Board_initGeneral();
    Board_initGPIO();
    Board_initSPI();
//    Board_initUART();
//    Board_initSDSPI();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

//    UART_start();
//    SD_init();

    System_printf("Board initialized\n");
    System_flush();

    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;

    taskParams.priority = 5;
    taskParams.stack = &taskStack;
    Task_construct(&taskStruct, (Task_FuncPtr)taskFxn, &taskParams, NULL);

    BIOS_start();    /* does not return */

    return(0);
}
