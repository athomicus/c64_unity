// bitmap.c
#include "bitmap.h"
#include <c64.h>
#include <string.h>

void BITMAP(const unsigned char* data) {
    // VIC bank 1: $4000-$7FFF
    *((unsigned char*)0xDD00) = (*((unsigned char*)0xDD00) & 0xFC) | 0x02;
    // bitmap @ $6000 (offset $2000 w banku 1)
    // screen @ $4400 (offset $0400 w banku 1)
    memcpy((unsigned char*)0x6000, data,        8000);
    memcpy((unsigned char*)0x4400, data + 8000, 1000);
    VIC.ctrl1 |= 0x20;    // hires bitmap ON
    VIC.ctrl2 &= ~0x10;   // nie multicolor
    VIC.addr   = 0x18;    // screen@offset$0400, bitmap@offset$2000
}

void BITMAP_OFF(void) {
    *((unsigned char*)0xDD00) = (*((unsigned char*)0xDD00) & 0xFC) | 0x03;
    VIC.ctrl1 &= ~0x20;
    VIC.addr   = 0x15;
}

void BITMAP_COLOR(unsigned char foreground, unsigned char background)
{
    unsigned int i;
    unsigned char colorByte = (foreground << 4) | (background & 0x0F);
    unsigned char *colorRam = (unsigned char*)0x4400;
    for (i = 0; i < 1000; i++)
        colorRam[i] = colorByte;
}


/*

BITMAP_COLOR(1,  0);   // biały na czarnym     — klasyczny
BITMAP_COLOR(5,  0);   // zielony na czarnym   — terminal
BITMAP_COLOR(7,  0);   // żółty na czarnym     — złoty
BITMAP_COLOR(4, 11);   // fioletowy na szarym  — fioletowo-szary
BITMAP_COLOR(3,  6);   // cyan na niebieskim   — zimny błękit
BITMAP_COLOR(13, 5);   // jasno-zielony na zielonym — dżungla
BITMAP_COLOR(10, 2);   // jasno-czerwony na czerwonym — ogień

0	0x0	⬛ Czarny
1	0x1	⬜ Biały
2	0x2	🟥 Czerwony
3	0x3	🩵 Cyan
4	0x4	🟣 Fioletowy
5	0x5	🟩 Zielony
6	0x6	🟦 Niebieski
7	0x7	🟨 Żółty
8	0x8	🟧 Pomarańczowy
9	0x9	🟫 Brązowy
10	0xA	🩷 Jasno-czerwony
11	0xB	🩶 Ciemnoszary
12	0xC	🩶 Szary (średni)
13	0xD	💚 Jasno-zielony
14	0xE	🔵 Jasno-niebieski
15	0xF	⬜ Jasnoszary

*/