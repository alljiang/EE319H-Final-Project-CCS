/*
 *  Audio Driver
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdio.h>
#include <xdc/std.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MENU_SOUND_BACKGROUND 0
#define SOUND_DEATHBLAST 10
#define STAGE_SOUND_BATTLEFIELD 11
#define STAGE_SOUND_EER 12
#define STAGE_SOUND_FINALDESTINATION 13
#define STAGE_SOUND_GREGORYGYM 14
#define STAGE_SOUND_SMASHVILLE 15
#define STAGE_SOUND_UTTOWER 16

#define KIRBY_SOUND_SQUAT 20
#define KIRBY_SOUND_RISE 21
#define KIRBY_SOUND_RUNSTART 22
#define KIRBY_SOUND_HURT1 23
#define KIRBY_SOUND_HURT2 24
#define KIRBY_SOUND_JUMP 25
#define KIRBY_SOUND_JAB 26
#define KIRBY_SOUND_FORWARDTILT 27
#define KIRBY_SOUND_DOWNTILT 28
#define KIRBY_SOUND_UPTILT 29
#define KIRBY_SOUND_FORWARDSMASH 30
#define KIRBY_SOUND_DOWNSMASH 31
#define KIRBY_SOUND_UPSMASH 32
#define KIRBY_SOUND_DASHATTACK 33
#define KIRBY_SOUND_FORWARDAIR 34
#define KIRBY_SOUND_DOWNAIR 35
#define KIRBY_SOUND_BACKAIR 36
#define KIRBY_SOUND_UPAIR 37
#define KIRBY_SOUND_NEUTRALAIR 38
#define KIRBY_SOUND_NEUTRALB 39
#define KIRBY_SOUND_DOWNB 40
#define KIRBY_SOUND_SIDEB 41
#define KIRBY_SOUND_UPB1 42
#define KIRBY_SOUND_UPB2 43
#define KIRBY_SOUND_UPB3 44
#define KIRBY_SOUND_UPB4 45
#define KIRBY_SOUND_UPB1VOICE 46
#define KIRBY_SOUND_UPB2VOICE 47
#define KIRBY_SOUND_UPB3VOICE 48
//#define KIRBY_SOUND_ 49
//#define KIRBY_SOUND_ 50
//#define KIRBY_SOUND_ 51
//#define KIRBY_SOUND_ 52
//#define KIRBY_SOUND_ 53
//#define KIRBY_SOUND_ 54
//#define KIRBY_SOUND_ 55
//#define KIRBY_SOUND_ 56
//#define KIRBY_SOUND_ 57
//#define KIRBY_SOUND_ 58
//#define KIRBY_SOUND_ 59
//#define KIRBY_SOUND_ 60

const char soundNames[200][7] = {
/*  0   */      {"menu"},
                {"321go"},
                {"smash"},
                {"sine"},
                {"game"},
                {""},
                {""},
                {""},
                {""},
                {""},
/*  10  */      {"mdead"},
                {"mbtfd"},
                {"meer"},
                {"mfdes"},
                {"mgreg"},
                {"msmvl"},
                {"mtowe"},
                {""},
                {""},
                {""},
/*  20  */      {"ksqua"},
                {"kris"},
                {"krnst"},
                {"kht1"},
                {"kht2"},
                {"kjmp"},
                {"kjab"},
                {"kftil"},
                {"kdtil"},
                {"kutil"},
/*  30  */      {"kfsma"},
                {"kdsma"},
                {"kusma"},
                {"kdsha"},
                {"kfai"},
                {"kdai"},
                {"kbai"},
                {"kuai"},
                {"knai"},
                {"kneub"},
/*  40  */      {"kdwnb"},
                {"ksidb"},
                {"kub1"},
                {"kub2"},
                {"kub3"},
                {"kub4"},
                {"kub1v"},
                {"kub2v"},
                {"kub3v"},
                {""},
/*  50  */      {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
/*  60  */      {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
/*  70  */      {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
/*  80  */      {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
/*  90  */      {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
/*  100  */      {""},
};

struct AudioParams {
    uint16_t soundIndex;
    uint32_t startIndex;
    int32_t endIndex;   // in frames. if endIndex == -1, play entire song
    uint32_t frames;
    int32_t FIFO_size;   // -1 means not set
    float volume;
    bool loop;
    FILE* file;
    int8_t* handlePointer;
};

//  DAC pins, smaller index == smaller resistor value
const uint32_t dac_pins[8][2] = {
                           {GPIO_PORTA_BASE, GPIO_PIN_2},
                           {GPIO_PORTE_BASE, GPIO_PIN_3},
                           {GPIO_PORTD_BASE, GPIO_PIN_6},
                           {GPIO_PORTC_BASE, GPIO_PIN_7},
                           {GPIO_PORTC_BASE, GPIO_PIN_6},
                           {GPIO_PORTB_BASE, GPIO_PIN_3},
                           {GPIO_PORTA_BASE, GPIO_PIN_7},
                           {GPIO_PORTF_BASE, GPIO_PIN_4},
};

extern void audioISR(UArg arg);
extern void ReadSDFIFO();
extern void Audio_init();
extern void Audio_initSD();
extern void Audio_closeSD();
extern int8_t Audio_playAudio(struct AudioParams sendable);
extern void Audio_play(uint16_t soundIndex, float volume, int8_t* handle = nullptr, uint32_t startIndex=0, int32_t endIndex=-1, bool loop=false);
extern void Audio_destroy(int8_t* slotID);
extern void Audio_destroyAudio(int8_t* slotID, bool overrideLoop);
extern void Audio_destroyAllAudio();
extern void Audio_clearBuffer();
extern void Audio_DAC_write(uint16_t mapping);
extern void Audio_initParams(struct AudioParams* params);

#ifdef __cplusplus
}
#endif

#endif
