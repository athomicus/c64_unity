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

    // Sprite data na początku banku — z dala od screen RAM ($4800)
    // Sprite 0: $4000, Sprite 1: $4040, ..., Sprite 7: $41C0
    unsigned int spriteBase = 0x4000 + (n * 64);
    for (i = 0; i < 64; i++)
        POKE(spriteBase + i, data[i]);

    // Pointer = (spriteBase - bank_start) / 64
    // ($4000 - $4000) / 64 = 0
    // Sprite 0 = 0, Sprite 1 = 1, ..., Sprite 7 = 7
    POKE(0x4BF8 + n, n);  // ← był 0x47F8 + n, 32 + n

    // Hi-res: wyzeruj bit N w rejestrze multicolor
    POKE(0xD01C, PEEK(0xD01C) & ~mask);

    POKE(0xD027 + n, color);
    POKE(0xD000 + (n * 2), (unsigned char)x);
    POKE(0xD001 + (n * 2), y);

    if (x > 255)
        POKE(0xD010, PEEK(0xD010) |  mask);
    else
        POKE(0xD010, PEEK(0xD010) & ~mask);

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