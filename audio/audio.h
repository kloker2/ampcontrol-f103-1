#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#include "audiodefs.h"

#define STEP_MULT   8

typedef enum {
    AUDIO_IC_NO = 0,
    AUDIO_IC_TDA7439,
    AUDIO_IC_TDA7313,
    AUDIO_IC_PT232X,

    AUDIO_IC_END
} AudioIC;

typedef struct {
    void (*setTune)(AudioTune tune, int8_t value);
    void (*setInput)(uint8_t value);

    void (*setMute)(bool value);

    void (*setPower)(bool value);
} AudioApi;

typedef struct {
    AudioIC ic;
    AudioApi api;
    AudioParam par;
} AudioProc;

void audioInit(void);
AudioProc *audioGet(void);
AudioParam *audioGetPar(void);

void audioSetPower(bool value);

void audioSetTune(AudioTune tune, int8_t value);
void audioChangeTune(AudioTune tune, int8_t diff);

void audioSetInput(uint8_t value);
void audioSetFlag(AudioFlag flag, bool value);


#endif // AUDIO_H
