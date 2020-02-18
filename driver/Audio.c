/*
 *  Audio Driver
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

/* XDC module Headers */
#include <driver/Board.h>
#include <xdc/std.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDSPI.h>
#include <ti/sysbios/knl/Clock.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "Audio.h"

#define AudioBitrate 44100
#define FIFOBufferSize 2000
#define NumAudioSlots 32

uint16_t audioFIFOBuffer[FIFOBufferSize];
uint16_t FIFO_Start = 0;    //  inclusive
uint16_t FIFO_Size = 0;

Clock_Struct audioClkStruct;
Clock_Struct SDClkStruct;

SDSPI_Handle sdspiHandle;
SDSPI_Params sdspiParams;
FILE *src;

struct AudioSendable audioSlots[NumAudioSlots];

void audioClkFxn(UArg arg0) {
    if(FIFO_Size == 0) return;

    //  write to DAC
    Audio_DAC_write(audioFIFOBuffer[FIFO_Start]);

    //  reset the FIFO location
    audioFIFOBuffer[FIFO_Start] = 0;

    //  increment to next location in FIFO
    FIFO_Start++;
    FIFO_Size--;

    //  wrap around FIFO_Start
    if(FIFO_Start == FIFOBufferSize) FIFO_Start = 0;

    //  add extra loops to compensate for faster frequency of clock fxn than 44.1khz
    uint8_t i;
    for(i = 0; i < 2; i++) {}
}

void SDClkFxn(UArg arg0) {
    uint8_t buffer[FIFOBufferSize/2+10];
    int32_t i;
    for(i = 0; i < NumAudioSlots; i++) {
        //  skip if audio finished or uninitialized
        if(audioSlots[i].startIndex == audioSlots[i].endIndex) {
            Audio_destroySendable(i);
            continue;
        }

        //  open file
        char systemFilename[75] = "fat:0:";
        char fileTail[6] = ".audio";
        strcat(systemFilename, sounds[audioSlots[i].soundIndex]);
        strcat(systemFilename, fileTail);
        src = fopen(systemFilename, "r");
        if(!src) { System_printf("File not found"); System_flush(); }

        //  read numFrames as a 32-bit integer
        fread(buffer, 4, 1, src);
        uint32_t numFrames =
                (buffer[0] << 24) +
                (buffer[1] << 16) +
                (buffer[2] << 8)  +
                (buffer[3]);
        audioSlots[i].frames = numFrames;

        //  if end index is undefined, play entire song
        if(audioSlots[i].endIndex == -1) audioSlots[i].endIndex = numFrames;

        //  destroy already played values
        fread(buffer, audioSlots[i].startIndex, 1, src);

        //  calculate how many bytes to read
        uint16_t bytesToRead = FIFOBufferSize/2;
        uint16_t totalBytesRemaining = audioSlots[i].endIndex - audioSlots[i].startIndex;
        if(bytesToRead > totalBytesRemaining) {
            bytesToRead = totalBytesRemaining;
        }

        //  read in bytes
        fread(buffer, bytesToRead, 1, src);

        //  add to FIFO buffer
        for(i = 0; i < bytesToRead; i++) {
            //  if buffer full, skip
            if(FIFO_Size == FIFOBufferSize) continue;

            audioFIFOBuffer[(FIFO_Start + FIFO_Size++) % FIFOBufferSize] += buffer[i];
        }

        fclose(src);
    }
}

void Audio_init() {
    //  Set GPIO pins
    uint8_t i;
    for(i = 0; i < 8; i++) {
        GPIOPinTypeGPIOOutput(dac_pins[i][0], dac_pins[i][1]);
    }

    //  Initialize periodic clock with period = 12us
    Clock_Params clkParams;
    Clock_Params_init(&clkParams);
    clkParams.period = 2;
    clkParams.startFlag = TRUE;

    Clock_construct(&audioClkStruct, (Clock_FuncPtr)audioClkFxn, 1, &clkParams);

    //  whenever FIFO is half-full/half-empty, read in the next set of audio
    clkParams.period = FIFOBufferSize * 100000 / 2 / AudioBitrate;
    clkParams.startFlag = TRUE;
    Clock_construct(&SDClkStruct, (Clock_FuncPtr)SDClkFxn, 1, &clkParams);

    //  Initialize SD Card
    SDSPI_Params_init(&sdspiParams);
    sdspiHandle = SDSPI_open(Board_SDSPI0, 0, &sdspiParams);
    if (sdspiHandle == NULL) { System_abort("Error starting the SD card\n"); }
    else { System_printf("SD Card is mounted\n"); }
    System_flush();

    for(i = 0; i < NumAudioSlots; i++) {
        Audio_destroySendable(i);
    }
}

//  returns the index of the sendable slot, -1 if all slots full
int8_t Audio_playSendable(struct AudioSendable sendable) {
    int8_t slot;
    //find first available slot
    for(slot = 0; slot < NumAudioSlots; slot++) {
        if(audioSlots[slot].startIndex == audioSlots[slot].endIndex) break;
    }
    if(slot >= NumAudioSlots) return -1;

    audioSlots[slot] = sendable;
    return slot;
}

void Audio_destroySendable(int8_t slotID) {
    if(slotID < 0) return;
    audioSlots[slotID].startIndex = 0;
    audioSlots[slotID].endIndex = 0;
}

//  8 bit, MSB = smallest resistance, greatest voltage
void Audio_DAC_write(uint16_t mapping) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        uint8_t output = (mapping >> (7-i)) & 1;
        if(output) output = dac_pins[i][1];
        GPIOPinWrite(dac_pins[i][0], dac_pins[i][1], output);
    }
}
