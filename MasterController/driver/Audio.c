/*
 *  Audio Driver
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

/* XDC module Headers */
#include <driver/Audio.h>
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

#include "Utils.h"

#define FIFOBufferSize 2048
#define audioMiddle 128

#define NumAudioSlots 12

const char fileHeader[] = "fat:0:";
const char fileTail[] = ".txt";

uint16_t audioFIFOBuffer[FIFOBufferSize];
volatile int32_t FIFO_Start = 0;    //  inclusive
volatile int32_t FIFO_Max_Size = 0; //  length of the longest audio

SDSPI_Handle Audio_sdspiHandle;
SDSPI_Params Audio_sdspiParams;
struct AudioParams audioSlots[NumAudioSlots];

uint8_t readBuffer[FIFOBufferSize];
volatile uint8_t numAudio = 0;

volatile int32_t samplesPlayed = 0;

void audioISR(UArg arg) {
    if(FIFO_Max_Size == 0 || numAudio == 0) {
        millis();
        return;
    }

//      write to DAC
    uint16_t toWrite = audioFIFOBuffer[FIFO_Start] / numAudio;
    if(toWrite > 0 && toWrite <= 255) {
        Audio_DAC_write(toWrite);
    }

    //  reset the FIFO location
    audioFIFOBuffer[FIFO_Start] = 0;

    //  increment to next location in FIFO
    FIFO_Start++;
    FIFO_Max_Size--;
    samplesPlayed++;

    //  wrap around FIFO_Start
    if(FIFO_Start == FIFOBufferSize) {
        FIFO_Start = 0;
    }
}

void ReadSDFIFO() {
    uint32_t FIFO_Start_original = FIFO_Start;
    uint32_t samplesPlayedSinceLastLoop = samplesPlayed;
    samplesPlayed = 0;
    uint8_t numAudioRead = 0;
    for(int16_t slot = 0; slot < NumAudioSlots; slot++) {
        //  skip if audio finished or uninitialized
        if(audioSlots[slot].startIndex == audioSlots[slot].endIndex) {
            //  only destroy the sendable if it has not been destroyed yet
            if(audioSlots[slot].startIndex != 0) {
                Audio_destroyAudio(slot);
            }
            continue;
        }

        //  set FIFO end position if this is the first run
        if(audioSlots[slot].FIFO_size < 0) {
            audioSlots[slot].FIFO_size = FIFO_Start;
        }
        else {
            //  change the FIFO size
            audioSlots[slot].FIFO_size -= samplesPlayedSinceLastLoop;
        }

        //  Get number of bytes to read between remaining size of FIFO buffer or remaining size of audio
        uint16_t bytesToRead = FIFOBufferSize-audioSlots[slot].FIFO_size;
        uint32_t totalBytesRemaining = audioSlots[slot].endIndex - audioSlots[slot].startIndex;
        if(totalBytesRemaining > 0) numAudioRead++;
        if(totalBytesRemaining < bytesToRead) {
            bytesToRead = totalBytesRemaining;
        }
        if(bytesToRead > 512) bytesToRead = 512;
//        if(bytesToRead >= 512) {
//            bytesToRead = 512*(bytesToRead/512);    // read in multiples of 512 bytes (1 page)
//        }

        //  read in bytes
        uint32_t bytesActuallyRead = fread(readBuffer, 1, bytesToRead, audioSlots[slot].file);
        if(bytesActuallyRead == 0) continue;    // something's wrong

        //  add to FIFO buffer
        uint32_t j;
        for(j = 0; j < bytesToRead; j++) {
            audioFIFOBuffer[(FIFO_Start_original + audioSlots[slot].FIFO_size++)
                            % FIFOBufferSize] +=
                                (uint8_t)(((int16_t)readBuffer[j] - audioMiddle) * audioSlots[slot].volume + audioMiddle);
        }

        //  move up audio starting index
        audioSlots[slot].startIndex += bytesToRead;

//          keep track of max size for the audio to stop when nothing's left
        if(FIFO_Max_Size < audioSlots[slot].FIFO_size) {
            FIFO_Max_Size = audioSlots[slot].FIFO_size;
        }
    }
    numAudio = numAudioRead;
}

void Audio_init() {

    //  Set GPIO pins
    for(uint8_t i = 0; i < 8; i++) {
        GPIOPinTypeGPIOOutput(dac_pins[i][0], dac_pins[i][1]);
    }
}

void Audio_initSD() {
    //  Initialize SD Card
   SDSPI_Params_init(&Audio_sdspiParams);
   Audio_sdspiHandle = SDSPI_open(Board_SDSPI0, 0, &Audio_sdspiParams);
   if (Audio_sdspiHandle == NULL) { System_abort("Error starting the SD card\n"); }
   else { System_printf("SD Card is mounted\n"); }
   System_flush();
}

/*
 * if program crashes while trying to play more audio, try increasing heap size in cfg
 * file or increasing task stack size
 */
//  returns the index of the sendable slot, -1 if all slots full
int8_t Audio_playAudio(struct AudioParams sendable) {
    int8_t slot;
    //find first available slot
    for(slot = 0; slot < NumAudioSlots; slot++) {
        if(audioSlots[slot].startIndex == audioSlots[slot].endIndex) break;
    }
    if(slot >= NumAudioSlots) return -1;    //  return -1 if all slots full

    //  open file
    char systemFilename[30];
    uint8_t strIndex = 0;
    for(uint32_t j = 0; fileHeader[j] != '\0'; j++) {    //  header
        systemFilename[strIndex++] = fileHeader[j];
    }
    for(uint32_t j = 0; soundNames[sendable.soundIndex][j] != '\0'; j++) {  //  filename body
        systemFilename[strIndex++] = soundNames[sendable.soundIndex][j];
    }
    for(uint32_t j = 0; fileTail[j] != '\0'; j++) {    //  tail/filetype
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

void Audio_destroyAudio(int8_t slotID) {
    if(slotID < 0 || slotID >= NumAudioSlots) return;
    audioSlots[slotID].startIndex = 0;
    audioSlots[slotID].endIndex = 0;

    if(audioSlots[slotID].file != NULL) {
        //  close file if open
        fclose(audioSlots[slotID].file);
    }
}

void Audio_destroyAllAudio() {
    for(uint8_t i = 0; i < NumAudioSlots; i++) {
        Audio_destroyAudio(i);
    }
}

//  8 bit, MSB = smallest resistance, greatest voltage
void Audio_DAC_write(uint16_t mapping) {
//    mapping = 0b11110000;
    if(mapping == 0) return;
    for(int8_t i = 7; i >= 0; i--) {
        uint8_t output = (mapping >> (i)) & 1;
        if(output) output = dac_pins[7-i][1];
        GPIOPinWrite(dac_pins[7-i][0], dac_pins[7-i][1], output);
    }
}

//  Sets default values of audio params
void Audio_initParams(struct AudioParams* params) {
    params->startIndex = 0;
    params->endIndex = -1;
    params->FIFO_size = -1;
    params->volume = 1;
}
