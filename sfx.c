#include "sfx.h"
#include <c64.h>
#include "unity.h"  

static unsigned char sfx_step  = 0;
static unsigned char sfx_timer = 0;
static unsigned char sfx_id    = SFX_NONE;

void SFX_Play(unsigned char id)
{
    sfx_id    = id;
    sfx_step  = 1;
    sfx_timer = 0;
}

void SFX_Update(void)
{
    if (sfx_step == 0) return;
    sfx_timer++;

    // ===================== SFX_RING =====================
    if (sfx_id == SFX_RING) {
        if (sfx_step == 1 && sfx_timer == 1) {
            POKE(0xD418, 0x0F);
            POKE(0xD405, 0x00); POKE(0xD406, 0xA0);
            POKE(0xD404, 0x10);
            POKE(0xD401, 0x20); POKE(0xD404, 0x11);
        }
        if (sfx_step == 1 && sfx_timer >= 5)  { sfx_step=2; sfx_timer=0; POKE(0xD404,0x10); }

        if (sfx_step == 2 && sfx_timer == 1)  { POKE(0xD401, 0x35); POKE(0xD404, 0x11); }
        if (sfx_step == 2 && sfx_timer >= 5)  { sfx_step=3; sfx_timer=0; POKE(0xD404,0x10); }

        if (sfx_step == 3 && sfx_timer == 1)  { POKE(0xD401, 0x55); POKE(0xD404, 0x11); }
        if (sfx_step == 3 && sfx_timer >= 8)  { sfx_step=0; POKE(0xD404,0x10); }
    }

    // // ===================== SFX_HURT =====================
    // if (sfx_id == SFX_HURT) {
    //     if (sfx_step == 1 && sfx_timer == 1) {
    //         POKE(0xD418, 0x0F);
    //         POKE(0xD405, 0x20); POKE(0xD406, 0x00);  // szybki decay
    //         POKE(0xD404, 0x10);
    //         POKE(0xD401, 0x08);  // niska nuta — ból
    //         POKE(0xD404, 0x21);  // szum + gate ON
    //     }
    //     if (sfx_step == 1 && sfx_timer >= 10) { sfx_step=0; POKE(0xD404,0x20); }
    // }

    // // ===================== SFX_LEVELUP =====================
    if (sfx_id == SFX_LEVELUP) {

    // Nuty: C-E-G-C (akord durowy w górę) + długi finał
    if (sfx_step == 1 && sfx_timer == 1) {
        POKE(0xD418, 0x0F);
        POKE(0xD405, 0x00); POKE(0xD406, 0xF0);  // A=0,D=0,S=F,R=0
        POKE(0xD404, 0x10);                        // reset
        POKE(0xD400, 0x00); POKE(0xD401, 0x11);  // C3
        POKE(0xD404, 0x11);                        // trójkąt ON
    }
    if (sfx_step == 1 && sfx_timer >= 4)
        { sfx_step=2; sfx_timer=0; POKE(0xD404,0x10); }

    if (sfx_step == 2 && sfx_timer == 1)
        { POKE(0xD401, 0x16); POKE(0xD404, 0x11); }  // E3
    if (sfx_step == 2 && sfx_timer >= 4)
        { sfx_step=3; sfx_timer=0; POKE(0xD404,0x10); }

    if (sfx_step == 3 && sfx_timer == 1)
        { POKE(0xD401, 0x1A); POKE(0xD404, 0x11); }  // G3
    if (sfx_step == 3 && sfx_timer >= 4)
        { sfx_step=4; sfx_timer=0; POKE(0xD404,0x10); }

    if (sfx_step == 4 && sfx_timer == 1)
        { POKE(0xD401, 0x22); POKE(0xD404, 0x11); }  // C4
    if (sfx_step == 4 && sfx_timer >= 4)
        { sfx_step=5; sfx_timer=0; POKE(0xD404,0x10); }

    // pauza
    if (sfx_step == 5 && sfx_timer >= 2)
        { sfx_step=6; sfx_timer=0; }

    // finałowy akord — dłuższy z wolnym release
    if (sfx_step == 6 && sfx_timer == 1) {
        POKE(0xD405, 0x02); POKE(0xD406, 0xF4);  // A=0,D=2,S=F,R=4
        POKE(0xD401, 0x44);                        // C5 — oktawa wyżej
        POKE(0xD404, 0x11);
    }
    if (sfx_step == 6 && sfx_timer >= 15)
        { sfx_step=7; sfx_timer=0; POKE(0xD404,0x10); }

    // release — czekaj aż wybrzmi
    if (sfx_step == 7 && sfx_timer >= 8)
        { sfx_step=0; }
}

}