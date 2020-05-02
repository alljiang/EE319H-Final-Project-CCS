
/* XDC module Headers */
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

#include "colors_fdst.h"
#include "colors_tower.h"
#include "colors_battlefield.h"
#include "colors_smashville.h"
#include "colors_gregory.h"
#include "colors_eer.h"
#include "colors_charmenu.h"
#include "colors_stagemenu.h"
#include "colors_w1gw.h"
#include "colors_w1gwc.h"
#include "colors_w1kb.h"
#include "colors_w1kbc.h"
#include "colors_w1va.h"
#include "colors_w1vac.h"
#include "colors_w2gw.h"
#include "colors_w2gwc.h"
#include "colors_w2kb.h"
#include "colors_w2kbc.h"
#include "colors_w2va.h"
#include "colors_w2vac.h"

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

    /*
    animator_initialize();
    animator_readCharacterSDCard(0);

    animator_setBackgroundColors(colors_fdst);
    animator_readPersistentSprite(stageNames[0], 0, 0);
    */

    /*
    int currentFrame = 0;
    while(true) {
        int t1 = millis();
        animator_animate(0, 1, 80, 100, currentFrame, 30, 0, 0, 1, false);
        animator_update();

        currentFrame++;
        currentFrame %= 8;

        int t2 = millis();

        sleep(50);
    }
    */

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
        buffer[0] = 0xAB;
        buffer[1] = 0xBC;
        UART_transmit(2, buffer);
        sleep(1000);
    }
    */

//    /*
    Flash_init();
    animator_initialize();

    SD_startSDCard();
    ILI9341_init();

    UART_sendByte(0x10);
    uint8_t buffer[20];
    while(1) {
        UART_receive(1, buffer);

        //  Read Character SD Card
        if(buffer[0] == 0x0A) {
            UART_receive(1, buffer);

            uint8_t characterIndex = buffer[0];

            animator_readCharacterSDCard(characterIndex);
        }
        //  Send Animation
        else if(buffer[0]  == 0x0B) {
            UART_receive(12, buffer);

            uint8_t characterIndex = buffer[0];
            uint8_t animationIndex = buffer[1];
            int16_t x = (buffer[2] << 8) | buffer[3];
            int16_t y = (buffer[4] << 8) | buffer[5];
            uint8_t frameIndex = buffer[6];
            bool persistent = buffer[7];
            uint8_t layer = buffer[8];
            bool continuous = buffer[9];
            uint8_t framePeriod = buffer[10];
            bool mirrored = buffer[11];

            animator_animate(characterIndex, animationIndex, x, y,
                             frameIndex, layer, persistent, continuous,
                             framePeriod, mirrored);
        }
        //  Set Background Colors
        else if(buffer[0] == 0x0C) {
            UART_receive(1, buffer);

            uint8_t stageIndex = buffer[0];

            if(stageIndex == STAGE_FINALDESTINATION) {
                animator_setBackgroundColors(colors_fdst);
            }
            else if(stageIndex == STAGE_TOWER) {
                animator_setBackgroundColors(colors_tower);
            }
            else if(stageIndex == STAGE_BATTLEFIELD) {
                animator_setBackgroundColors(colors_battle);
            }
            else if(stageIndex == STAGE_SMASHVILLE) {
                animator_setBackgroundColors(colors_smashville);
            }
            else if(stageIndex == STAGE_EER) {
                animator_setBackgroundColors(colors_eer);
            }
            else if(stageIndex == STAGE_GREGORYGYM) {
                animator_setBackgroundColors(colors_gregory);
            }
            else if(stageIndex == BACKGROUND_MENU) {
                animator_setBackgroundColors(colors_charmenu);
            }
            else if(stageIndex == BACKGROUND_STAGEMENU) {
                animator_setBackgroundColors(colors_stagemenu);
            }
            else if(stageIndex == BACKGROUND_WIN_P1_KIRBY) {
                animator_setBackgroundColors(colors_w1kb);
            }
            else if(stageIndex == BACKGROUND_WIN_P2_KIRBY) {
                animator_setBackgroundColors(colors_w2kb);
            }
            else if(stageIndex == BACKGROUND_WIN_P1_GAMEANDWATCH) {
                animator_setBackgroundColors(colors_w1gw);
            }
            else if(stageIndex == BACKGROUND_WIN_P2_GAMEANDWATCH) {
                animator_setBackgroundColors(colors_w2gw);
            }
            else if(stageIndex == BACKGROUND_WIN_P1_VALVANO) {
                animator_setBackgroundColors(colors_w1va);
            }
            else if(stageIndex == BACKGROUND_WIN_P2_VALVANO) {
                animator_setBackgroundColors(colors_w2va);
            }
            else if(stageIndex == BACKGROUND_WIN_P1_KIRBY_CHINESE) {
                animator_setBackgroundColors(colors_w1kbc);
            }
            else if(stageIndex == BACKGROUND_WIN_P2_KIRBY_CHINESE) {
                animator_setBackgroundColors(colors_w2kbc);
            }
            else if(stageIndex == BACKGROUND_WIN_P1_GAMEANDWATCH_CHINESE) {
                animator_setBackgroundColors(colors_w1gwc);
            }
            else if(stageIndex == BACKGROUND_WIN_P2_GAMEANDWATCH_CHINESE) {
                animator_setBackgroundColors(colors_w2gwc);
            }
            else if(stageIndex == BACKGROUND_WIN_P1_VALVANO_CHINESE) {
                animator_setBackgroundColors(colors_w1vac);
            }
            else if(stageIndex == BACKGROUND_WIN_P2_VALVANO_CHINESE) {
                animator_setBackgroundColors(colors_w2vac);
            }
        }
        //  Read Persistent Sprite
        else if(buffer[0] == 0x0D) {
            UART_receive(4, buffer);

            uint8_t spriteIndex = buffer[0];
            uint16_t x = (buffer[1] << 8) | buffer[2];
            uint8_t y = buffer[3];

            animator_readPersistentSprite(persistentSprites[spriteIndex], x, y);
        }
        //  Reset
        else if(buffer[0] == 0x0E) {
//            Flash_resetCurrentMemoryLocation();
            Flash_releaseSPI();
            Flash_init();
            animator_initialize();
            SD_releaseSDCard();
            SD_startSDCard();
            ILI9341_fillScreen(0);
        }
        //  Animator Update
        else if(buffer[0] == 0xFE) {
            animator_update();
        }

        //  Send acknowledge byte
        UART_sendByte(0x10);
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

    System_printf("Board initialized\n");
    System_flush();

    Task_Params params;
    Task_Params_init(&params);
    params.stackSize = 8000;

    params.priority = 1;
    params.stack = &st;
    Task_construct(&ts, (Task_FuncPtr)start, &params, NULL);

    // debug pin
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);

    BIOS_start();    /* does not return */

    return(0);
}
