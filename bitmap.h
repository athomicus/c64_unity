// bitmap.h
#ifndef BITMAP_H
#define BITMAP_H

// Stala: uzyj oryginalnego koloru z danych kafla
#define TILE_COLOR_AUTO 255

// Kolory C64 — wygodne stale
#define C64_BLACK       0
#define C64_WHITE       1
#define C64_RED         2
#define C64_CYAN        3
#define C64_PURPLE      4
#define C64_GREEN       5
#define C64_BLUE        6
#define C64_YELLOW      7
#define C64_ORANGE      8
#define C64_BROWN       9
#define C64_LIGHTRED    10
#define C64_DARKGRAY    11
#define C64_GRAY        12
#define C64_LIGHTGREEN  13
#define C64_LIGHTBLUE   14
#define C64_LIGHTGRAY   15


void DRAW_TILE(const unsigned char* data,
               unsigned char dest_cx, unsigned char dest_cy,
               unsigned char cells_w, unsigned char cells_h,
               unsigned char fg,      unsigned char bg);

void DRAW_TILE_FLIP_H(const unsigned char* data,
                      unsigned char dest_cx, unsigned char dest_cy,
                      unsigned char cells_w, unsigned char cells_h,
                      unsigned char fg,      unsigned char bg);

#endif/* BITMAP_H */
