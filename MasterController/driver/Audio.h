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
#define MENU_SOUND_CONFIRM  5
#define MENU_SOUND_DESELECT  6
#define MENU_SOUND_SELECT  7
#define MENU_SOUND_GAMESTART  8
#define MENU_SOUND_CLAP  9

#define SOUND_DEATHBLAST 10
#define STAGE_SOUND_BATTLEFIELD 11
#define STAGE_SOUND_EER 12
#define STAGE_SOUND_FINALDESTINATION 13
#define STAGE_SOUND_GREGORYGYM 14
#define STAGE_SOUND_SMASHVILLE 15
#define STAGE_SOUND_UTTOWER 16

#define KIRBY_SOUND_CROUCH 20
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

#define GAW_SOUND_CROUCH 49
#define GAW_SOUND_RISE 50
#define GAW_SOUND_STEP 51
#define GAW_SOUND_JUMP 52
#define GAW_SOUND_DOUBLEJUMP 53
#define GAW_SOUND_LANDING 54
#define GAW_SOUND_NEUTRALATTACK 55
#define GAW_SOUND_FORWARDTILT 56
#define GAW_SOUND_DOWNTILT 57
#define GAW_SOUND_UPTILT 58
#define GAW_SOUND_SMASHHOLDSTART 59
#define GAW_SOUND_SMASHATTACK 60
#define GAW_SOUND_FORWARDAIR 61
#define GAW_SOUND_BACKAIR 62
#define GAW_SOUND_UPAIR 63
#define GAW_SOUND_DOWNAIR 64
#define GAW_SOUND_NEUTRALAIR 65
#define GAW_SOUND_NEUTRALB 66
#define GAW_SOUND_SIDEB 67
#define GAW_SOUND_UPB 68
#define GAW_SOUND_DOWNB 69
#define GAW_SOUND_DOWNBABSORB 70
#define GAW_SOUND_DOWNBATTACK 71

#define SOUND_SMASHCHARGE 72
#define SOUND_HIT1 73
#define SOUND_HIT2 74
#define SOUND_HIT3 75
#define SOUND_HIT4 76
#define SOUND_SHIELDUP 77
#define SOUND_SHIELDDOWN 78
#define SOUND_SHIELDBREAK 79
#define SOUND_CROWDCHEER 80
#define SOUND_LEDGECATCH 81
//#define SOUND_ 82
//#define SOUND_ 83
//#define SOUND_ 84
//#define SOUND_ 85
//#define SOUND_ 86

#define VALVANO_SOUND_LINE_HIIMJONVALVANO 90
#define VALVANO_SOUND_LINE_LETSGOFLYING 91
#define VALVANO_SOUND_LINE_THEREYOUGO 92
#define VALVANO_SOUND_LINE_TAKETHIS 93
//#define VALVANO_SOUND_ 94
//#define VALVANO_SOUND_ 95
//#define VALVANO_SOUND_ 96
//#define VALVANO_SOUND_ 97
//#define VALVANO_SOUND_ 98
//#define VALVANO_SOUND_ 99
#define VALVANO_SOUND_CROUCH 100
#define VALVANO_SOUND_RISE 101
#define VALVANO_SOUND_STEP 102
#define VALVANO_SOUND_JUMP 103
#define VALVANO_SOUND_LANDING 104
#define VALVANO_SOUND_HURT1 105
#define VALVANO_SOUND_HURT2 106
#define VALVANO_SOUND_RUNSTART 107
#define VALVANO_SOUND_JAB 108
#define VALVANO_SOUND_FORWARDTILT 109
#define VALVANO_SOUND_UPTILT 110
#define VALVANO_SOUND_DOWNTILT 111
#define VALVANO_SOUND_FORWARDAIR 112
#define VALVANO_SOUND_NEUTRALAIR 113
#define VALVANO_SOUND_UPAIR 114
#define VALVANO_SOUND_DOWNAIR 115
#define VALVANO_SOUND_DASHATTACK 116
#define VALVANO_SOUND_LASER 117
#define VALVANO_SOUND_UPSPECIAL 118
#define VALVANO_SOUND_DROPCAR 119
#define VALVANO_SOUND_EXPLOSION 120

const char soundNames[200][7] = {
/*  0   */      {"menu"},
                {"321go"},
                {"smash"},
                {"sine"},
                {"game"},
                {"mcnfm"},
                {"mdslc"},
                {"mslct"},
                {"mgmst"},
                {"mclap"},
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
                {"gcrou"},
/*  50  */      {"grise"},
                {"gstep"},
                {"gjmp"},
                {"gdjmp"},
                {"gland"},
                {"gnatt"},
                {"gftil"},
                {"gdtil"},
                {"gutil"},
                {"gsmst"},
/*  60  */      {"gsmsh"},
                {"gfair"},
                {"gbair"},
                {"guair"},
                {"gdair"},
                {"gnair"},
                {"gneub"},
                {"gsidb"},
                {"gupb"},
                {"gdnb"},
/*  70  */      {"gdbab"},
                {"gdbat"},
                {"mchrg"},
                {"mhit1"},
                {"mhit2"},
                {"mhit3"},
                {"mhit4"},
                {"mshup"},
                {"mshdn"},
                {"mshbk"},
/*  80  */      {"mcrdc"},
                {"mldge"},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
/*  90  */      {"vhijv"},
                {"vlgfl"},
                {"vtygo"},
                {"vtkth"},
                {""},
                {""},
                {""},
                {""},
                {""},
                {""},
/*  100  */     {"vcrou"},
                {"vrise"},
                {"vstep"},
                {"vjump"},
                {"vland"},
                {"vhrt1"},
                {"vhrt2"},
                {"vruns"},
                {"vjab"},
                {"vftil"},
/*  110 */      {"vutil"},
                {"vdtil"},
                {"vfair"},
                {"vnair"},
                {"vuair"},
                {"vdair"},
                {"vdsat"},
                {"vlasr"},
                {"vuspe"},
                {"vdrpc"},
/*  120 */      {"vexpl"},
                {""},
                {""},
                {""},
                {""},
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
extern void ReadSDFIFO(UArg arg);
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
