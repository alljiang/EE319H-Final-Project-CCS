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
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "Audio.h"
#include "Utils.h"

#define AudioBitrate 44100
#define FIFOBufferSize 100
#define NumAudioSlots 32

const char fileHeader[] = "fat:0:";
const char fileTail[] = ".txt";

bool FIFOBufferInUse = false;

uint16_t audioFIFOBuffer[FIFOBufferSize];
uint16_t FIFO_Start = 0;    //  inclusive
uint16_t FIFO_Size = 0;

Clock_Struct audioClkStruct;
Clock_Struct SDClkStruct;

SDSPI_Handle sdspiHandle;
SDSPI_Params sdspiParams;
struct AudioSendable audioSlots[32];

uint8_t buffer[250];

uint16_t misses = 0;
uint16_t hits = 0;

void audioClkFxn(UArg arg0) {

    if(FIFO_Size == 0) {
        return;
    }

    if(FIFOBufferInUse) {
        misses++;
        return;
    }
    FIFOBufferInUse = true;
    hits++;

    //  write to DAC
    Audio_DAC_write(audioFIFOBuffer[FIFO_Start]);

    //  reset the FIFO location
    audioFIFOBuffer[FIFO_Start] = 0;

    //  increment to next location in FIFO
    FIFO_Start++;
    FIFO_Size--;

    //  wrap around FIFO_Start
    if(FIFO_Start == FIFOBufferSize) FIFO_Start = 0;

    FIFOBufferInUse = false;

    //  add extra loops to compensate for faster frequency of clock fxn than 44.1khz
//    uint8_t i;
//    for(i = 0; i < 2; i++) {}
}

void SDClkFxn() {
    int32_t slot;
    int32_t t1 = micros();
    for(slot = 0; slot < NumAudioSlots; slot++) {
        //  skip if audio finished or uninitialized
        if(audioSlots[slot].startIndex == audioSlots[slot].endIndex) {
            //  only destroy the sendable if it has not been destroyed yet
            if(audioSlots[slot].startIndex != 0) Audio_destroySendable(slot);
            continue;
        }

        //  Get number of bytes to read between remaining size of FIFO buffer or remaining size of audio
        uint16_t bytesToRead = FIFOBufferSize-FIFO_Size;
        uint32_t totalBytesRemaining = audioSlots[slot].endIndex - audioSlots[slot].startIndex;
        if(totalBytesRemaining < bytesToRead) {
            bytesToRead = totalBytesRemaining;
        }

        //  read in bytes
        fread(buffer, 1, bytesToRead, audioSlots[slot].file);

        if(FIFOBufferInUse) continue;
        FIFOBufferInUse = true;

        //  add to FIFO buffer
        uint32_t j;
        for(j = 0; j < bytesToRead; j++) {
            //  if buffer full, skip
            if(FIFO_Size == FIFOBufferSize) continue;

            audioFIFOBuffer[(FIFO_Start + FIFO_Size++) % FIFOBufferSize] += buffer[j];
        }

        FIFOBufferInUse = false;

        //  move up audio starting index
        audioSlots[slot].startIndex += bytesToRead;
    }
    int32_t t2 = micros();
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
    clkParams.period = 9;
    clkParams.startFlag = TRUE;

    Clock_construct(&audioClkStruct, (Clock_FuncPtr)audioClkFxn, 1, &clkParams);

    //  whenever FIFO is half-full/half-empty, read in the next set of audio
//    clkParams.period = FIFOBufferSize * 100000 / 2 / AudioBitrate;
//    clkParams.period = 200;
//    clkParams.startFlag = TRUE;
//    Clock_construct(&SDClkStruct, (Clock_FuncPtr)SDClkFxn, 1, &clkParams);

}

void Audio_initSD() {
    //  Initialize SD Card
   SDSPI_Params_init(&sdspiParams);
   sdspiHandle = SDSPI_open(Board_SDSPI0, 0, &sdspiParams);
   if (sdspiHandle == NULL) { System_abort("Error starting the SD card\n"); }
   else { System_printf("SD Card is mounted\n"); }
   System_flush();
}

//  returns the index of the sendable slot, -1 if all slots full
int8_t Audio_playSendable(struct AudioSendable sendable) {
    int8_t slot;
    //find first available slot
    for(slot = 0; slot < NumAudioSlots; slot++) {
        if(audioSlots[slot].startIndex == audioSlots[slot].endIndex) break;
    }
    if(slot >= NumAudioSlots) return -1;    //  return -1 if all slots full

    //  open file
    char systemFilename[30];
    uint8_t strIndex = 0;
    uint32_t j;
    for(j = 0; fileHeader[j] != '\0'; j++) {    //  header
        systemFilename[strIndex++] = fileHeader[j];
    }
    for(j = 0; soundNames[sendable.soundIndex][j] != '\0'; j++) {  //  filename body
        systemFilename[strIndex++] = soundNames[sendable.soundIndex][j];
    }
    for(j = 0; fileTail[j] != '\0'; j++) {    //  tail/filetype
        systemFilename[strIndex++] = fileTail[j];
    }
    systemFilename[strIndex] = '\0';

    sendable.file = fopen(systemFilename, "r");

    if(!sendable.file) {
        System_printf("File not found or is busy");
        System_flush();
        return -1;
    }

    fread(buffer, 4, 1, sendable.file);
    uint32_t numFrames =
            (buffer[0] << 24) +
            (buffer[1] << 16) +
            (buffer[2] << 8)  +
            (buffer[3]);
    sendable.frames = numFrames;

    //  if end index is undefined, play entire song
    if(sendable.endIndex == -1) sendable.endIndex = numFrames;

    audioSlots[slot] = sendable;
    return slot;
}

void Audio_destroySendable(int8_t slotID) {
    if(slotID < 0) return;
    audioSlots[slotID].startIndex = 0;
    audioSlots[slotID].endIndex = 0;

    if(audioSlots[slotID].file != NULL) {
        //  close file if open
        fclose(audioSlots[slotID].file);
    }
}

//  8 bit, MSB = smallest resistance, greatest voltage
void Audio_DAC_write(uint16_t mapping) {
    System_printf("\"%d\"\n", mapping);
    System_flush();
    uint8_t i;
    for(i = 0; i < 8; i++) {
        uint8_t output = (mapping >> (7-i)) & 1;
        if(output) output = dac_pins[i][1];
        GPIOPinWrite(dac_pins[i][0], dac_pins[i][1], output);
    }
}
