
#include <Flash.h>
#include <xdc/runtime/System.h>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "animator.h"
#include "metadata.h"
#include "SD.h"
#include "utils.h"
#include "ILI9341.h"
#include "colors.h"
#include <cmath>
using namespace std;

#define maxSprites 16

uint8_t buffer[1000];   // it's big because i can. if this is too big, lower it down to like 700
uint8_t* bufferptr;

uint8_t smallBuffer2[300];
int32_t finalColors[321];  // a color index of -1 means 'do not change'
uint8_t layer[321];
bool rowsToUpdate[241];
const uint32_t *backgroundColors;

uint32_t backgroundColorIndex = 0;

uint32_t persistentBackgroundMemLocation;
Animation animation[4][numberOfAnimations];
SpriteSendable spriteSendables[maxSprites]; // up to maxSprites sprites on screen at once
uint16_t activeAnimations = 0;  // each bit represents if the corresponding sendable is being used, big endian
uint16_t toRemove = 0;  // each bit represents if the corresponding sendable should be removed next update, big endian

void printArr(uint16_t size, uint8_t* buf) {
    for(uint16_t  i = 0; i < size; i++) {
        printf("%d,", buf[i]);
    }
    printf("\n");
}

void printStr(uint16_t size, uint8_t* buf) {
    for(uint16_t  i = 0; i < size; i++) {
        printf("%c,", buf[i]);
    }
    printf("\n");
}

//  returns number of bytes read
uint16_t readUntil(char delimeter, uint8_t* buf) {
    uint16_t characters = 0;
    do { buf[characters++] = SD_readNextChar(); }
    while(buf[characters-1] != delimeter);
    return characters-1;
}

//  Big endian!
uint16_t readHalfInt(uint8_t* buf) {
    return (buf[0] << 8u) + buf[1];
}

//  updates screen line by line, segment by segment using animation data
//  1.25 KB
void animator_update() {
    for(uint8_t row = 0; row <= 240; row++) {
        if(!rowsToUpdate[row]) continue;
        rowsToUpdate[row] = false;

        // set all color indexes to -2 initially for 'do not change'
        // set all layers to background
        for(uint16_t col = 0; col <= 320; col++) {
            finalColors[col] = ANIMATOR_COLOR_DONOTCHANGE;
            layer[col] = LAYER_BACKGROUND;
        }

        //  first, loop through toRemoves and set as -1 background index (need to change)
        for(uint8_t slot = 0; slot < maxSprites; slot++) {
            //  check if this slot should be removed, and continue only if it should be
            if((toRemove >> slot) & 1) {
                //  get the animation (pointer just to not make another copy)
                SpriteSendable* ss = &spriteSendables[slot];
                Animation* anim = &animation[ss->charIndex][ss->animationIndex];

                //  see if this current row intersects this sprite animation
                int16_t heightDifference = row - ss->y;
                if(heightDifference < 0 || heightDifference >= anim->height) {
                    // this row is out of bounds of this animation, skip it
                    continue;
                }

                // loop through entire width
                for(uint16_t col = 0; col < anim->width; col++) {
                    //  set the color indexes to the background color
                    if(ss->mirrored) {
                        if(ss->x+(anim->width) - col > 320 || ss->x+(anim->width) - col < 0) continue;
                        finalColors[ss->x + (anim->width) - col] = ANIMATOR_COLOR_BACKGROUND;
                    }
                    else {
                        if(ss->x + col > 320 || ss->x + col < 0) continue;
                        finalColors[ss->x + col] = ANIMATOR_COLOR_BACKGROUND;
                    }
                }
            }
        }

        //  next, loop through the actives and get their data
        for(uint8_t slot = 0; slot < 16; slot++) {
            //  check if this slot should be removed, and continue only if it shouldn't be
            if( ( (activeAnimations >> slot) & 1u ) ) {
                //  get the animation (pointer just to not make another copy)
                SpriteSendable* ss = &spriteSendables[slot];
                Animation* anim = &animation[ss->charIndex][ss->animationIndex];

                //  see if this current row intersects this sprite animation
                int16_t heightDifference = row - ss->y;
                if(heightDifference < 0 || heightDifference >= anim->height) {
                    // this row is out of bounds of this animation, skip it and move on to the next sprite
                    continue;
                }

                //  This row intersects! Now, paint this row of this animation into the color index buffer

                //  Make sure to update this row next loop to remove sprite
                rowsToUpdate[row] = true;
                //  get frame location with frame index array
                bufferptr = Flash_readMemory(anim->memLocation + 3*ss->frame, 3, buffer);
                uint32_t frameLocation = anim->memLocation  //  start location
                        + anim->frames*3    //  frame index array
                        + (anim->height+1)*2 * ss->frame    //  row index array
                        + ((bufferptr[0] << 16u) + (bufferptr[1] << 8u) + bufferptr[2])*2;    //  frame location offset

                //  get row location with row index array
                bufferptr = Flash_readMemory(frameLocation + (anim->height - heightDifference - 1)*2, 4, buffer);
                uint32_t rowStartOffset = (bufferptr[0] << 8u) + bufferptr[1];
                uint32_t rowSize = ((bufferptr[2] << 8u) + bufferptr[3] - rowStartOffset);

                uint32_t rowStartLocation = frameLocation + (anim->height+1)*2 + rowStartOffset;

                //  read the row of data into the buffer
                bufferptr = Flash_readMemory(rowStartLocation, rowSize, buffer);

                //  copy the data over from the buffer into the color index buffer
                uint16_t column = 0;
                uint16_t numPairs = rowSize >> 1u;
                for(uint16_t pair = 0; pair < numPairs; pair++) {
                    uint16_t colorIndex = (bufferptr[pair*2+0]);
                    uint16_t quantity = (bufferptr[pair*2+1]);

                    if(colorIndex == backgroundColorIndex) {
                        //  this is the background color, ignore it
                        column += quantity;
                        continue;
                    }

                    //  add the colorIndex to colorIndexes quantity times, update the layer there too
                    while(quantity-- > 0) {
                        if(ss->mirrored) {
                            if(!(ss->x+(anim->width) - column > 320 || ss->x+(anim->width) - column < 0)) {
                                if (layer[ss->x + (anim->width) - column] < ss->layer) {
                                    finalColors[ss->x + (anim->width) - column] = colors[colorIndex];
                                    layer[ss->x + (anim->width) - column] = ss->layer;
                                }
                            }
                        }
                        else {
                            if(!(ss->x + column > 320 || ss->x + column < 0)) {
                                if (layer[ss->x + column] < ss->layer) {
                                    finalColors[ss->x + column] = colors[colorIndex];
                                    layer[ss->x + column] = ss->layer;
                                }
                            }
                        }
                        column++;
                    }
                }
            }
        }

        //  next, replace all the -1s with the background from Flash background reserve
        for(uint16_t col = 0; col <= 320; col++) {
            if(finalColors[col] != ANIMATOR_COLOR_BACKGROUND) continue;

            //  if there are -1s in a row, read them together to save time
            uint16_t consecutiveBackgroundRowSize = 1;
            while((col + consecutiveBackgroundRowSize) < 320) {
                if(col + consecutiveBackgroundRowSize > 320) continue;
                if(finalColors[col + consecutiveBackgroundRowSize] == ANIMATOR_COLOR_BACKGROUND) {
                    consecutiveBackgroundRowSize++;
                }
                else break;
            }

            //  read in from Flash
            uint32_t backgroundRowLocation = persistentBackgroundMemLocation + row * 321 * 2 + col * 2;
            bufferptr = Flash_readMemory(backgroundRowLocation, 2*consecutiveBackgroundRowSize, buffer);

            //  copy from buffer into colorIndexes array
            for(uint16_t i = 0; i < consecutiveBackgroundRowSize; i++) {
                if(col+i > 321 || col + i < 0) continue;
                finalColors[col + i] = backgroundColors[(bufferptr[i*2] << 8u) + (bufferptr[i*2 + 1])];
            }

            col += consecutiveBackgroundRowSize-1;
        }

        //  finally, draw it!

        //  send in segments divided by "Do not change" colors (-2s)
        for(uint16_t col = 0; col <= 320; col++) {
            if(finalColors[col] == ANIMATOR_COLOR_DONOTCHANGE) continue;

            //  find height of segment
            uint16_t consecutiveSize = 1;
            while((col + consecutiveSize) < 320) {
                if(finalColors[col + consecutiveSize] != ANIMATOR_COLOR_DONOTCHANGE) {
                    consecutiveSize++;
                }
                else break;
            }

            //  write this section into the LCD
            ILI9341_drawColors(col, row, finalColors + col, consecutiveSize);

            col += consecutiveSize -1;
        }
    }

    //  clear all toRemove flags
    toRemove = 0;

    //  flag all non-persistent sprite animations to move to next frame and be erased next update
    for(uint8_t slot = 0; slot < 16; slot++) {
        if((activeAnimations >> slot) & 1) {
            SpriteSendable* ss = &spriteSendables[slot];
            Animation* anim = &animation[ss->charIndex][ss->animationIndex];

            // animation active
            if(!ss->persistent) {
                //  if this is supposed to last longer, let it last longer
                if(ss->framePeriod - ++ss->currentframeLength > 0) continue;
                else ss->currentframeLength = 0;

                ss->frame += 1;
                if((ss->frame >= anim->frames) || !ss->continuous) {
                    activeAnimations &= ~(1u << slot);
                }
                toRemove |= (1u << slot);
            }
        }
    }
    millis();
}

void animator_setBackgroundColors(const uint32_t *backgroundArr) {
    backgroundColors = backgroundArr;
}

//  receive from UART, adds an animation to be displayed
void animator_animate(uint8_t charIndex, uint8_t animationIndex,
        int16_t x, int16_t y, uint8_t frame, uint8_t animationlayer, uint8_t persistent,
        uint8_t continuous, uint8_t framePeriod, bool mirrored) {

    //  find first unused animation slot
    uint8_t slot;
    for(slot = 0; slot < 16; slot++) {
        bool inUse = (activeAnimations >> slot) & 1 || (toRemove >> slot) & 1;
        if(!inUse) {
            //  we about to start using it, so flag it as in use
            activeAnimations |= (1u << slot);
            break;
        }
    }

    spriteSendables[slot].charIndex = charIndex;
    spriteSendables[slot].animationIndex = animationIndex;
    spriteSendables[slot].x = x;
    spriteSendables[slot].y = y;
    spriteSendables[slot].frame = frame;
    spriteSendables[slot].layer = animationlayer;
    spriteSendables[slot].persistent = persistent;
    spriteSendables[slot].continuous = continuous;
    spriteSendables[slot].framePeriod = framePeriod;
    spriteSendables[slot].currentframeLength = 0;
    spriteSendables[slot].mirrored = mirrored;

    for(int16_t i = y; i < y+animation[charIndex][animationIndex].height; i++) {
        if(i >= 0 && i < 241) rowsToUpdate[i] = true;
    }

//    if(spriteSendables[slot].persistent) {
//        //  add to persistent array in Flash
//
//        //  paint row by row
//        for(uint8_t row = y; row < y + animation[charIndex][animationIndex].height; row++) {
//            Flash_readMemory(animation[charIndex][animationIndex].memLocation,
//                            animation[charIndex][animationIndex].width, buffer);
//
//            uint32_t memInsertLocation = persistentBackgroundMemLocation + row * 321 + x;
//
//            Flash_writeMemory_specifiedAddress(memInsertLocation, animation[charIndex][animationIndex].width, buffer);
//        }
//    }
}

void animator_initialize() {
    persistentBackgroundMemLocation = Flash_allocateMemory(241*321*3);

    // Find which color index is 0xFFFFFFFF (background)
    for(int32_t i = 0; i < sizeof(colors); i++) {
        if(colors[i] == 0xFFFFFFFF) {
            backgroundColorIndex = i;
            break;
        }
    }

    for(uint8_t i = 0; i < 241; i++) rowsToUpdate[i] = false;

    SD_startSDCard();
    Flash_init();
    ILI9341_init();
}

void animator_readPersistentSprite(const char* spriteName, uint16_t x, uint8_t y) {
    char filename[40] = "data/sprites/";
    char fileType[] = ".txt";

    strcat(filename, spriteName);
    strcat(filename, fileType);

    SD_openFile(filename);

    printf("Reading in sprite: %s\n", spriteName);


    SD_read(2, buffer);
    uint16_t width = readHalfInt(buffer);

    SD_read(1, buffer);
    uint8_t height = buffer[0];

    //  get the data and store it
    for(int16_t row = height-1; row >= 0; row--) {
        SD_read(width*2, buffer);

        uint32_t FlashRowLocation = persistentBackgroundMemLocation + (row-y) * 321*2 + x;
        Flash_writeMemory_specifiedAddress(FlashRowLocation, width*2, buffer);

        //  assemble indexes
        for (uint32_t i = 0; i < 321; i++) {
            finalColors[i] = backgroundColors[(buffer[2 * i] << 8u) + buffer[2 * i +1]];
        }

        ILI9341_drawColors(x, row, finalColors, width);
    }
    SD_closeFile();
}

void animator_readCharacterSDCard(uint8_t charIndex) {
    char filename[40] = "data/sprites/";
    char fileType[] = ".txt";

    strcat(filename, characterNames[charIndex]);
    strcat(filename, fileType);

    SD_openFile(filename);

    //  get number of animations
    SD_read(2, buffer);
    uint16_t numAnimations = readHalfInt(buffer);

    System_printf("Reading in character %s: %d animations\n", characterNames[charIndex], numAnimations);
    System_flush();

    uint8_t animationName[15];
    while(numAnimations--) {
        //  get reference index of this animation name inside metadata
        uint16_t animationIndex;
        uint8_t animationNameLength = readUntil('\n', animationName);

        //  loop through all the animation names of this character
        bool found = false;
        for (animationIndex = 0; animationIndex < numberOfAnimations; animationIndex++) {
            bool matchSoFar = true;
            //  loop through the animation index name. If there's a mismatch, move on to the next animation name
            for (uint16_t i = 0; i < animationNameLength; i++) {
                if (animationName[i] != animations[charIndex][animationIndex][i]) {
                    matchSoFar = false;
                    break;
                }
            }
            if (matchSoFar) {
                found = true;
                break;
            }
        }
        if(!found) {
            animationName[animationNameLength] = '\0'; //  prepare to print the string
            System_printf("ERROR Animation not found: %s\n", animationName);
            System_flush();
            while(1);
            continue;
        }
        else {
            animationName[animationNameLength] = '\0'; //  prepare to print the string
        }

        Animation *anim = &animation[charIndex][animationIndex];

        //  construct animation struct
        anim->animationIndex = animationIndex;
        anim->characterIndex = charIndex;

        SD_read(1, buffer);
        anim->frames = buffer[0];

        SD_read(2, buffer);
        anim->width = readHalfInt(buffer);

        SD_read(1, buffer);
        anim->height = buffer[0];

        //  get the frame indexes, store into Flash and smallBuffer2
        SD_read(anim->frames*3, smallBuffer2);
        anim->memLocation = Flash_writeMemory(anim->frames*3, smallBuffer2);

        //  get the data of each frame and store it
        for (uint8_t f = 0; f < anim->frames; f++) {
            SD_read(2*(anim->height+1), buffer);
            Flash_writeMemory(2 * (anim->height + 1), buffer);
            uint32_t frameDataSize = (buffer[2 * anim->height] << 8u) + buffer[2 * anim->height + 1];
            uint32_t bytesToRead = frameDataSize;

            // read and write frame data in chunks
            uint16_t maxChunkSize = 100;
            while (bytesToRead > 0) {
                uint16_t chunkSize = bytesToRead;
                if (chunkSize > maxChunkSize) chunkSize = maxChunkSize;

                SD_read(chunkSize, buffer);
                Flash_writeMemory(chunkSize, buffer);

                bytesToRead -= chunkSize;
            }
        }
    }
    SD_closeFile();
}
