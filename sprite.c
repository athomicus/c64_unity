// sprite.c
#include "sprite.h"
#include <c64.h>
#include <string.h>
#include "unity.h"

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

    // $4800 + n*64 — przed bitmapa, z dala od $6000
    // Sprite 0: $4800, Sprite 1: $4840, ..., Sprite 7: $49C0
    unsigned int spriteBase = 0x4800 + (n * 64);
    for (i = 0; i < 64; i++)
        POKE(spriteBase + i, data[i]);

    // Pointer = offset od poczatku banku / 64
    // ($4800 - $4000) / 64 = $800 / 64 = 32
    // Sprite 0 = 32, Sprite 1 = 33, ..., Sprite 7 = 39
    POKE(0x47F8 + n, 32 + n);

    // Hi-res: wyzeruj bit N w rejestrze multicolor ($D01C)
    POKE(0xD01C, PEEK(0xD01C) & ~mask);

    // Kolor sprite'a N ($D027 + n)
    POKE(0xD027 + n, color);

    // Pozycja X i Y
    POKE(0xD000 + (n * 2), (unsigned char)x);
    POKE(0xD001 + (n * 2), y);

    if (x > 255)
        POKE(0xD010, PEEK(0xD010) |  mask);
    else
        POKE(0xD010, PEEK(0xD010) & ~mask);

    // Wlacz sprite N
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