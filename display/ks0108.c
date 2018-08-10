#include "ks0108.h"

#include "../pins.h"
#include "../functions.h"

#include "gm128x64.h"

DisplayDriver drv = {
    .setBrightness = ks0108SetBrightness,
    .drawPixel = ks0108DrawPixel,
    .drawFontChar = glcdDrawFontChar,
};

#ifdef _KS0108B
#define KS0108_SET_CS1()    CLR(KS0108_CS1); SET(KS0108_CS2)
#define KS0108_SET_CS2()    SET(KS0108_CS1); CLR(KS0108_CS2)
#define KS0108_SET_CS()     CLR(KS0108_CS2); CLR(KS0108_CS1)
#else
#define KS0108_SET_CS1()    SET(KS0108_CS1); CLR(KS0108_CS2)
#define KS0108_SET_CS2()    CLR(KS0108_CS1); SET(KS0108_CS2)
#define KS0108_SET_CS()     SET(KS0108_CS1); SET(KS0108_CS2)
#endif

static uint8_t fb[KS0108_COLS * KS0108_CHIPS][KS0108_ROWS];
static uint8_t _br;

void ks0108SetBrightness(uint8_t br)
{
    _br = br;
}

static void ks0108SetPort(uint8_t data)
{
    if (data & (1 << 0)) SET(KS0108_D0);
    else CLR(KS0108_D0);
    if (data & (1 << 1)) SET(KS0108_D1);
    else CLR(KS0108_D1);
    if (data & (1 << 2)) SET(KS0108_D2);
    else CLR(KS0108_D2);
    if (data & (1 << 3)) SET(KS0108_D3);
    else CLR(KS0108_D3);
    if (data & (1 << 4)) SET(KS0108_D4);
    else CLR(KS0108_D4);
    if (data & (1 << 5)) SET(KS0108_D5);
    else CLR(KS0108_D5);
    if (data & (1 << 6)) SET(KS0108_D6);
    else CLR(KS0108_D6);
    if (data & (1 << 7)) SET(KS0108_D7);
    else CLR(KS0108_D7);
}

static void ks0108SetDdrIn()
{
    IN_U(KS0108_D0);
    IN_U(KS0108_D1);
    IN_U(KS0108_D2);
    IN_U(KS0108_D3);
    IN_U(KS0108_D4);
    IN_U(KS0108_D5);
    IN_U(KS0108_D6);
    IN_U(KS0108_D7);
}

static void ks0108SetDdrOut()
{
    OUT(KS0108_D0);
    OUT(KS0108_D1);
    OUT(KS0108_D2);
    OUT(KS0108_D3);
    OUT(KS0108_D4);
    OUT(KS0108_D5);
    OUT(KS0108_D6);
    OUT(KS0108_D7);
}

static uint8_t ks0108ReadPin()
{
    uint8_t ret = 0;

    if (READ(KS0108_D0)) ret |= (1 << 0);
    if (READ(KS0108_D1)) ret |= (1 << 1);
    if (READ(KS0108_D2)) ret |= (1 << 2);
    if (READ(KS0108_D3)) ret |= (1 << 3);
    if (READ(KS0108_D4)) ret |= (1 << 4);
    if (READ(KS0108_D5)) ret |= (1 << 5);
    if (READ(KS0108_D6)) ret |= (1 << 6);
    if (READ(KS0108_D7)) ret |= (1 << 7);

    return ret;
}

static void ks0108WriteCmd(uint8_t cmd)
{
    _delay_us(50);

    CLR(KS0108_DI);
    ks0108SetPort(cmd);

    SET(KS0108_E);
    _delay_us(1);
    CLR(KS0108_E);
}

void ks0108IRQ()
{
    static uint8_t i;
    static uint8_t j;
    static uint8_t cs;

    static uint8_t br;

    // TODO: avoid function call, use direct port read
    drv.bus = ks0108ReadPin();                      // Read pins
    ks0108SetDdrOut();                              // Set data lines as outputs

    if (j == KS0108_PHASE_SET_PAGE) {               // Phase 1 (Y)
        if (++i >= 8) {
            i = 0;
            if (++cs >= KS0108_CHIPS)
                cs = 0;
            switch (cs) {
            case 1:
                KS0108_SET_CS2();
                break;
            default:
                KS0108_SET_CS1();
                break;
            }
        }
        CLR(KS0108_DI);                             // Go to command mode
        ks0108SetPort(KS0108_SET_PAGE + i);
    } else if (j == KS0108_PHASE_SET_ADDR) {        // Phase 2 (X)
        ks0108SetPort(KS0108_SET_ADDRESS);
    } else {                                        // Phase 3 (32 bytes of data)
        ks0108SetPort(fb[j + 64 * cs][i]);
    }

    SET(KS0108_E);                                  // Data strob
    _delay_us(1);
    CLR(KS0108_E);

    // Prepare to read pins
    ks0108SetPort(0xFF);                            // Set 1 (pull-up) on data lines
    ks0108SetDdrIn();                               // Set data lines as inputs

    if (++j > KS0108_PHASE_SET_ADDR) {
        j = 0;
        SET(KS0108_DI);                             // Go to data mode
    }

    if (++br >= KS0108_MAX_BRIGHTNESS)              // Loop brightness
        br = KS0108_MIN_BRIGHTNESS;

    if (br == _br) {
        CLR(KS0108_BCKL);                           // Turn backlight off
    } else if (br == 0)
        SET(KS0108_BCKL);                           // Turn backlight on
}

void ks0108Init(DisplayDriver **disp)
{
    *disp = &drv;
    gm128x64Init(*disp);

    // Set RW line to zero
    CLR(KS0108_RW);

    CLR(KS0108_DI);
    CLR(KS0108_E);

    // Hardware reset
    CLR(KS0108_RES);
    _delay_us(1);
    SET(KS0108_RES);
    _delay_us(1);

    // Init both controller
    KS0108_SET_CS();
    ks0108WriteCmd(KS0108_DISPLAY_START_LINE);
    ks0108WriteCmd(KS0108_DISPLAY_ON);
    _delay_ms(10);

    // Go to data mode
    SET(KS0108_DI);

    // Enable backlight control
    OUT(KS0108_BCKL);
}

void ks0108Clear()
{
    uint8_t i, j;

    for (i = 0; i < KS0108_COLS * KS0108_CHIPS; i++) {
        for (j = 0; j < KS0108_ROWS; j++) {
            fb[i][j] = 0x00;
        }
    }
}

void ks0108DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    uint8_t bit;

    if (x >= KS0108_COLS * KS0108_CHIPS)
        return;
    if (y >= KS0108_ROWS * 8)
        return;

    bit = 1 << (y & 0x07);

    if (color)
        fb[x][y >> 3] |= bit;
    else
        fb[x][y >> 3] &= ~bit;
}
