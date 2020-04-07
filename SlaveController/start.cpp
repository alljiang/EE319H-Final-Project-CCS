
/* XDC module Headers */
#include <colors_fdst.h>
#include <colors_tower.h>
#include <Flash.h>
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

#include "ILI9341.h"
#include "UART.h"
#include "Utils.h"
#include "SD.h"
#include "animator.h"

#include "metadata.h"

Task_Struct ts;

Char st[8000];

void start(UArg arg0, UArg arg1)
{
    /*
    ILI9341_init();

//    while(1) {
//        ILI9341_fillScreen(0);
//        Task_sleep(1000);
//        ILI9341_fillScreen(0xFFFFFFF);
//        Task_sleep(1000);
//    }

    ILI9341_fillScreen(0xFFFFFFF);
    int32_t arr[256];
    for(int i = 0; i < 256; i++) {
        arr[i] = i;
    }
    for(int i = 0; i < 30; i++) {
        ILI9341_drawColors(30, 20+i, arr, 256);
    }

    for(int i = 0; i < 256; i++) {
        arr[i] = i<<8;
    }
    for(int i = 0; i < 30; i++) {
        ILI9341_drawColors(30, 50+i, arr, 256);
    }

    for(int i = 0; i < 256; i++) {
        arr[i] = i<<16;
    }
    for(int i = 0; i < 30; i++) {
        ILI9341_drawColors(30, 80+i, arr, 256);
    }
    */

    /*
    Flash_init();

    uint8_t buffy[300];
    for(int i = 0; i < 300; i++) {
        buffy[i] = i;
    }

    Flash_writeMemory_specifiedAddress(0, 300, buffy);

    memset(buffy, 0x88, 100);

    Flash_readMemory(0, 300, buffy);
    Flash_readSFDP(buffy);
     */

//    uint8_t buffer[100];
//    while(1) {
//        UART_receive(10, buffer);
//    }

//    /*
    animator_initialize();
    animator_readCharacterSDCard(0);

    animator_setBackgroundColors(colors_fdst);
    animator_readPersistentSprite(stageNames[0], 0, 0);

    animator_animate(0, 7, 80, 100, 0, 30, 0, 0, 1, false);
    animator_update();
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

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    UART_start();

    System_printf("Board initialized\n");
    System_flush();

    Task_Params params;
    Task_Params_init(&params);
    params.stackSize = 8000;

    params.priority = 1;
    params.stack = &st;
    Task_construct(&ts, (Task_FuncPtr)start, &params, NULL);

    BIOS_start();    /* does not return */

    return(0);
}
