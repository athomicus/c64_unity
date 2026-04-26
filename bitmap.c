// bitmap.c
#include "bitmap.h"
#include <c64.h>
#include <string.h>
 




void DRAW_TILE(const unsigned char* data,
               unsigned char dest_cx, unsigned char dest_cy,
               unsigned char cells_w, unsigned char cells_h,
               unsigned char fg,      unsigned char bg)
{
    unsigned char cx, cy;
    unsigned char orig_color;
    unsigned char final_color;
    unsigned int  color_offset;
    unsigned int  tile_cell;
    unsigned int  dest_cell;

    color_offset = (unsigned int)cells_w * cells_h * 8;

    for (cy = 0; cy < cells_h; cy++) {
        if (dest_cy + cy >= 25) break;
        for (cx = 0; cx < cells_w; cx++) {
            if (dest_cx + cx >= 40) break;

            tile_cell = (unsigned int)cy * cells_w + cx;
            dest_cell = (unsigned int)(dest_cy + cy) * 40 + (dest_cx + cx);

            // Kopiuj 8 bajtow pikseli
            memcpy((unsigned char*)0x6000 + dest_cell * 8,
                   data + tile_cell * 8, 8);

            // Ustal kolor komorki
            orig_color = *(data + color_offset + tile_cell);

            if (fg == TILE_COLOR_AUTO && bg == TILE_COLOR_AUTO) {
                // Oba AUTO -> uzyj oryginalnych z danych kafla
                final_color = orig_color;
            } else if (fg == TILE_COLOR_AUTO) {
                // Tylko fg z oryginalu, bg z parametru
                final_color = (orig_color & 0xF0) | (bg & 0x0F);
            } else if (bg == TILE_COLOR_AUTO) {
                // Tylko bg z oryginalu, fg z parametru
                final_color = (fg << 4) | (orig_color & 0x0F);
            } else {
                // Oba podane recznie
                final_color = (fg << 4) | (bg & 0x0F);
            }

            *((unsigned char*)0x4800 + dest_cell) = final_color;
        }
    }
}
// ============================================================
//  DRAW_TILE_FLIP_H — rysuje kafla z odbiciem poziomym (lustro)
//
//  data     = tablica kafla (format identyczny jak DRAW_TILE)
//  dest_cx  = kolumna docelowa (0-39)
//  dest_cy  = wiersz docelowy  (0-24)
//  cells_w  = szerokosc kafla w komorkach
//  cells_h  = wysokosc kafla w komorkach
//  fg, bg   = kolory (lub TILE_COLOR_AUTO)
// ============================================================
void DRAW_TILE_FLIP_H(const unsigned char* data,
                      unsigned char dest_cx, unsigned char dest_cy,
                      unsigned char cells_w, unsigned char cells_h,
                      unsigned char fg,      unsigned char bg)
{
    unsigned char cx, cy, py;
    unsigned char orig_color, final_color;
    unsigned char src_byte, flipped_byte;
    unsigned int  color_offset;
    unsigned int  src_cell, dest_cell;

    color_offset = (unsigned int)cells_w * cells_h * 8;

    for (cy = 0; cy < cells_h; cy++) {
        if (dest_cy + cy >= 25) break;
        for (cx = 0; cx < cells_w; cx++) {
            if (dest_cx + cx >= 40) break;

            // Zrodlo: czytamy od PRAWEJ strony kafla
            src_cell  = (unsigned int)cy * cells_w + (cells_w - 1 - cx);
            // Cel: piszemy od LEWEJ (normalnie)
            dest_cell = (unsigned int)(dest_cy + cy) * 40 + (dest_cx + cx);

            // Kopiuj 8 bajtow pikseli z odbiciem bitow w kazdym bajcie
            for (py = 0; py < 8; py++) {
                src_byte = *(data + src_cell * 8 + py);

                // Odbij bity w bajcie (flip poziomy pikseli)
                flipped_byte  = 0;
                flipped_byte |= (src_byte & 0x80) >> 7;
                flipped_byte |= (src_byte & 0x40) >> 5;
                flipped_byte |= (src_byte & 0x20) >> 3;
                flipped_byte |= (src_byte & 0x10) >> 1;
                flipped_byte |= (src_byte & 0x08) << 1;
                flipped_byte |= (src_byte & 0x04) << 3;
                flipped_byte |= (src_byte & 0x02) << 5;
                flipped_byte |= (src_byte & 0x01) << 7;

                *((unsigned char*)0x6000 + dest_cell * 8 + py) = flipped_byte;
            }

            // Kolor — przy flipie fg i bg sie zamieniaja miejscami
            orig_color = *(data + color_offset + src_cell);

            if (fg == TILE_COLOR_AUTO && bg == TILE_COLOR_AUTO) {
                // Zamien fg<->bg bo lustro odwraca znaczenie bitow
                final_color = ((orig_color & 0x0F) << 4) | ((orig_color & 0xF0) >> 4);
            } else if (fg == TILE_COLOR_AUTO) {
                final_color = (orig_color & 0xF0) | (bg & 0x0F);
            } else if (bg == TILE_COLOR_AUTO) {
                final_color = (fg << 4) | (orig_color & 0x0F);
            } else {
                final_color = (fg << 4) | (bg & 0x0F);
            }

            *((unsigned char*)0x4800 + dest_cell) = final_color;
        }
    }
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