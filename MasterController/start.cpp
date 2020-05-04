
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
#include "v_tm4c123gh6pm.h"

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

Task_Struct taskStruct;
char stack[SIZE];

void start(UArg arg0, UArg arg1)
{
    /*
    while(1) {
        Controller_updateController(0);
        Controller_flagOldData();
    }
    */

//    /*
    Audio_initSD();
    Audio_destroyAllAudio();

//    while(1) {
//        ReadSDFIFO();
//        if(millis() - asdf > 5000) {
//            Audio_playAudio(audioparams);
//            audioparams.soundIndex = 0;
//            asdf = millis();
//        }
//    }

//    */

//    /*
    UART_waitForAcknowledge();
    startup();
    while(1) {
        Controller_updateController(0);
        Controller_flagOldData();
        loop();
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

    UART_start(&ReadSDFIFO);
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

    GPIO_PORTF_DIR_R |= 0x02;
    GPIO_PORTF_DATA_R &= ~0x02;

    BIOS_start();    /* does not return */

    return(0);
}
