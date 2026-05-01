// sprite.c
#include "sprite.h"
#include <c64.h>
#include <string.h>
#include "unity.h"
// Zastąp memcpy 8-bajtowym unrollem — cc65 to zoptymalizuje do LDA/STA


// Mapa Banku 1 ($4000-$7FFF):
//   $4400-$47E7  Screen RAM (kolory bitmapy)      1000B
//   $47F8-$47FF  Sprite pointery                     8B
//   $4800-$49FF  Dane sprite'ow 0-7 (8x64B)        512B  <-- BEZPIECZNE
//   $4A00-$5FFF  wolne                             5632B
//   $6000-$7D3F  Bitmapa piksele                   8000B

void SPRITE(unsigned char n, unsigned char *data, unsigned int x, unsigned char y, unsigned char color)
{
    unsigned char i;
    unsigned char mask = (1 << n);

    // Sprite data pod $4C00 — bezpiecznie poza programem i screen RAM
    unsigned int spriteBase = 0x4C00 + (n * 64);
    for (i = 0; i < 64; i++)
        POKE(spriteBase + i, data[i]);

    // Pointer: ($4C00 - $4000) / 64 = 48
    POKE(0x4BF8 + n, 48 + n);

    POKE(0xD01C, PEEK(0xD01C) & ~mask);
    POKE(0xD027 + n, color);
    POKE(0xD000 + (n * 2), (unsigned char)x);
    POKE(0xD001 + (n * 2), y);

    if (x > 255) POKE(0xD010, PEEK(0xD010) |  mask);
    else         POKE(0xD010, PEEK(0xD010) & ~mask);

    POKE(0xD015, PEEK(0xD015) | mask);
}

void SPRITE_MOVE(unsigned char n, unsigned int x, unsigned char y)
{
    unsigned char mask = (1 << n);

    POKE(0xD000 + (n * 2), (unsigned char)x);
    POKE(0xD001 + (n * 2), y);

    if (x > 255) POKE(0xD010, PEEK(0xD010) |  mask);
    else         POKE(0xD010, PEEK(0xD010) & ~mask);
}