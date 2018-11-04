#ifndef ACTIONS_H
#define ACTIONS_H

#include <stdint.h>

#define STBY_EXIT       0
#define STBY_ENTER      1
#define STBY_SWITCH     2

#define ACTION_HIDDEN   0
#define ACTION_VISIBLE  1

typedef enum {
    ACTION_NONE,

    ACTION_BTN_SHORT,
    ACTION_BTN_LONG,

    ACTION_STANDBY,
    ACTION_INIT_HW,

    ACTION_PREV,
    ACTION_NEXT,
    ACTION_OK,
    ACTION_BACK,

    ACTION_RTC_MODE,
    ACTION_RTC_CHANGE,
    ACTION_RTC_SET_HOUR,
    ACTION_RTC_SET_MIN,
    ACTION_RTC_SET_SEC,
    ACTION_RTC_SET_DATE,
    ACTION_RTC_SET_MONTH,
    ACTION_RTC_SET_YEAR,

    ACTION_AUDIO_INPUT,
    ACTION_AUDIO_PARAM_CHANGE,

    ACTION_AUDIO_VOLUME,
    ACTION_AUDIO_BASS,
    ACTION_AUDIO_MIDDLE,
    ACTION_AUDIO_TREBLE,
    ACTION_AUDIO_FRONTREAR,
    ACTION_AUDIO_BALANCE,
    ACTION_AUDIO_CENTER,
    ACTION_AUDIO_SUBWOOFER,
    ACTION_AUDIO_PREAMP,

    ACTION_AUDIO_GAIN,

    ACTION_TUNER_PREV,
    ACTION_TUNER_NEXT,

    ACTION_BR_WORK,
    ACTION_BR_STBY,

    ACTION_DISP_EXPIRED,

    ACTION_MENU_SELECT,
    ACTION_MENU_CHANGE,

    ACTION_RC_CMD,

    ACTION_TYPE_END
} ActionType;

typedef struct {
    ActionType type;
    int16_t value;
} Action;

Action actionUserGet(void);
void actionHandle(Action action, uint8_t visible);

#endif // ACTIONS_H
