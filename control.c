#include "control.h"

#include <stm32f1xx_ll_usart.h>
#include <stdbool.h>
#include <string.h>

#include "actions.h"
#include "debug.h"
#include "ringbuf.h"
#include "usart.h"
#include "utils.h"

#define CMDBUF_SIZE     64

#define CTRL_AMP        "amp"

#define CTRL_STBY       "stby"

#define CTRL_ENTER      "enter"
#define CTRL_EXIT       "exit"
#define CTRL_TOGGLE     "toggle"

static RingBuf cmdRb;
static char cmdRbData[CMDBUF_SIZE];
static LineParse cmdLp;

static void controlParseAmpStby(char *line)
{
    if (strstr(line, CTRL_ENTER) == line) {
        actionQueue(ACTION_STANDBY, FLAG_ENTER);
    } else if (strstr(line, CTRL_EXIT) == line) {
        actionQueue(ACTION_STANDBY, FLAG_EXIT);
    } else if (strstr(line, CTRL_TOGGLE)) {
        actionQueue(ACTION_STANDBY, FLAG_SWITCH);
    }
}

static void controlParseAmp(char *line)
{
    if (strstr(line, CTRL_STBY ".")) {
        controlParseAmpStby(line + sizeof(CTRL_STBY));
    }
}

static void controlParseLine(char *line)
{
    if (strstr(line, CTRL_AMP ".") == line) {
        controlParseAmp(line + sizeof(CTRL_AMP));
    }
}

void controlInit(void)
{
    ringBufInit(&cmdRb, cmdRbData, sizeof(cmdRbData));
}

void controlIRQ(void)
{
    char data = LL_USART_ReceiveData8(USART_DBG);

#ifdef _DEBUG_KARADIO
    usartSendChar(USART_KARADIO, data);
#endif

    ringBufPushChar(&cmdRb, data);
}

void controlGetData(void)
{
    uint16_t size = ringBufGetSize(&cmdRb);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&cmdRb);
        if (utilReadChar(&cmdLp, ch)) {
            controlParseLine(cmdLp.line);
        }
    }
}
