/*
 *  Audio Driver
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

/* XDC module Headers */
#include "rom.h"
#include <driver/Board.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDSPI.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Timer.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"

#include "Audio.h"
#include "Utils.h"

#define AudioBitrate 8000
#define FIFOBufferSize 512

#define NumAudioSlots 32

const char fileHeader[] = "fat:0:";
const char fileTail[] = ".txt";

bool FIFOBufferInUse = false;

uint16_t audioFIFOBuffer[FIFOBufferSize];
uint16_t FIFO_Start = 0;    //  inclusive
uint16_t FIFO_Size = 0;
uint32_t g_ui32Flags;

SDSPI_Handle Audio_sdspiHandle;
SDSPI_Params Audio_sdspiParams;
struct AudioSendable audioSlots[32];

uint8_t readBuffer[FIFOBufferSize];

uint8_t nextSound = 0;
bool readingSD = false;

void audioISR(UArg arg) {

    if(FIFO_Size == 0) {
//        ReadSDFIFO();
        return;
    }

    //  write to DAC
    Audio_DAC_write(audioFIFOBuffer[FIFO_Start]);

    //  reset the FIFO location
    audioFIFOBuffer[FIFO_Start] = 0;

    //  increment to next location in FIFO
    FIFO_Start++;
    FIFO_Size--;

    //  wrap around FIFO_Start
    if(FIFO_Start == FIFOBufferSize) FIFO_Start = 0;

//    Audio_DAC_write(nextSound);
//    nextSound = 0;
//    int32_t slot;
//    for(slot = 0; slot < NumAudioSlots; slot++) {
//        //  skip if audio finished or uninitialized
//        if(audioSlots[slot].startIndex == audioSlots[slot].endIndex) {
//            //  only destroy the sendable if it has not been destroyed yet
//            if(audioSlots[slot].startIndex != 0) Audio_destroySendable(slot);
//            continue;
//        }
//        fread(buffer, 1, 1, audioSlots[slot].file);
//
//        nextSound += buffer[0];
//
//        //  move up audio starting index
//        audioSlots[slot].startIndex += 1;
//    }
//
//    //  add extra loops to compensate for faster frequency of clock fxn than 44.1khz
//    uint8_t i;
//    for(i = 0; i < 63; i++) {}
}

void ReadSDFIFO() {
    int32_t slot;
    uint32_t maxBytesRead = 0;
    for(slot = 0; slot < NumAudioSlots; slot++) {
        uint32_t bytesRead = 0;
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
        if(bytesToRead >= 512) {
            bytesToRead = 512*(bytesToRead/512);    // read in multiples of 512 bytes (1 page)
        }

        //  read in bytes
        int32_t t1 = micros();
        fread(readBuffer, 1, bytesToRead, audioSlots[slot].file);
        int32_t t2 = micros();

        //  add to FIFO buffer
        uint32_t j;
        for(j = 0; j < bytesToRead; j++) {
            audioFIFOBuffer[(FIFO_Start + FIFO_Size) % FIFOBufferSize] += readBuffer[j];
            FIFO_Size++;
            bytesRead++;
        }

        //  move up audio starting index
        audioSlots[slot].startIndex += bytesToRead;

        if(bytesRead > maxBytesRead) maxBytesRead = bytesRead;
    }
//    FIFO_Size += maxBytesRead;
}

void Audio_init() {

    //  Set GPIO pins
//    uint8_t i;
//    for(i = 0; i < 8; i++) {
//        GPIOPinTypeGPIOOutput(dac_pins[i][0], dac_pins[i][1]);
//    }
    GPIOPinTypeGPIOOutput(dac_pins[0][0], dac_pins[0][1]);
    GPIOPinTypeGPIOOutput(dac_pins[1][0], dac_pins[1][1]);
    GPIOPinTypeGPIOOutput(dac_pins[2][0], dac_pins[2][1]);
    GPIOPinTypeGPIOOutput(dac_pins[3][0], dac_pins[3][1]);
    GPIOPinTypeGPIOOutput(dac_pins[4][0], dac_pins[4][1]);
    GPIOPinTypeGPIOOutput(dac_pins[5][0], dac_pins[5][1]);
    GPIOPinTypeGPIOOutput(dac_pins[6][0], dac_pins[6][1]);
    GPIOPinTypeGPIOOutput(dac_pins[7][0], dac_pins[7][1]);

    //  Set up timer
//    Timer_Params timerParams;
//    Timer_Handle myTimer;
//    Error_Block eb;

//    Error_init(&eb);
//    Timer_Params_init(&timerParams);
//    timerParams.period = 10;
//    timerParams.periodType = Timer_PeriodType_MICROSECS;
//    timerParams.arg = 1;
//    myTimer = Timer_create(Timer_ANY, audioISR, &timerParams, &eb);
//    if (myTimer == NULL) { System_abort("Timer create failed"); }
}

void Audio_initSD() {
    //  Initialize SD Card
   SDSPI_Params_init(&Audio_sdspiParams);
   Audio_sdspiHandle = SDSPI_open(Board_SDSPI0, 0, &Audio_sdspiParams);
   if (Audio_sdspiHandle == NULL) { System_abort("Error starting the SD card\n"); }
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

    fread(readBuffer, 4, 1, sendable.file);
    uint32_t numFrames =
            (readBuffer[0] << 24) +
            (readBuffer[1] << 16) +
            (readBuffer[2] << 8)  +
            (readBuffer[3]);
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

int count = 0;
//  8 bit, MSB = smallest resistance, greatest voltage
void Audio_DAC_write(uint16_t mapping) {
//    System_printf("\"%d\"\n", mapping);
//    System_flush();
//    if(mapping == 0) return;
    uint8_t i;
    for(i = 0; i < 8; i++) {
        uint8_t output = (mapping >> (7-i)) & 1;
        if(output) output = dac_pins[i][1];
        GPIOPinWrite(dac_pins[i][0], dac_pins[i][1], output);
    }
}
