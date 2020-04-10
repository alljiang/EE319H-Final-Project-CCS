
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

#include "UART.h"
#include "Utils.h"
#include "SD.h"
#include "Audio.h"
#include "Controller.h"
#include "game.h"


void start(UArg arg0, UArg arg1)
{
//
//    while(1) {
//        ReadSDFIFO();
//        sleep(1);
//    }

    /*
    uint8_t buffer[50];
    while(1) {
        UART_receive(2, buffer);
    }
    */

    /*
    uint8_t buffer[2];
    uint8_t a = 0;
    while(1) {
        buffer[0] = 0xDD;
        buffer[1] = 0xDD;
        UART_transmit(2, buffer);
        sleep(1000);
    }
    */

//    /*
    Audio_initSD();
    Audio_destroyAllAudio();

    AudioParams audioparams;
    Audio_initParams(&audioparams);

    audioparams.soundIndex = 0;
    audioparams.volume = 0.5;
    audioparams.loop = true;
    int8_t background = Audio_playAudio(audioparams);

    /*
    while(1) { ReadSDFIFO(); }
    */
//    */

//    /*
    UART_waitForAcknowledge();
    game_startup();
    int t1, t2;
    while(1) {
        Controller_updateController();
        game_loop();
        ReadSDFIFO();
    }
//    */
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
    Controller_init();

    Audio_init();

    System_printf("Board initialized\n");
    System_flush();

    Task_Params params;
    Task_Params_init(&params);
    params.stackSize = SIZE;

    params.priority = 1;
    params.stack = &stack;
    Task_construct(&taskStruct, (Task_FuncPtr)start, &params, NULL);

    BIOS_start();    /* does not return */

    return(0);
}
