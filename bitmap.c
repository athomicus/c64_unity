// bitmap.c
#include "bitmap.h"
#include <c64.h>
#include <string.h>
 

//screen 40x25  1=8px

void BITMAP(const unsigned char* data) {
    *((unsigned char*)0xDD00) = (*((unsigned char*)0xDD00) & 0xFC) | 0x02;
    memcpy((unsigned char*)0x6000, data,        8000);
    memcpy((unsigned char*)0x4400, data + 8000, 1000);
    VIC.ctrl1 |= 0x20;
    VIC.ctrl2 &= ~0x10;
    VIC.addr   = 0x18;
}

void BITMAP_OFF(void) {
    *((unsigned char*)0xDD00) = (*((unsigned char*)0xDD00) & 0xFC) | 0x03;
    VIC.ctrl1 &= ~0x20;
    VIC.addr   = 0x15;
}

void BITMAP_COLOR(unsigned char foreground, unsigned char background) {
    unsigned int i;
    unsigned char colorByte = (foreground << 4) | (background & 0x0F);
    unsigned char *colorRam = (unsigned char*)0x4400;
    for (i = 0; i < 1000; i++)
        colorRam[i] = colorByte;
}

void BITMAP_TILE(const unsigned char* data,
                 unsigned char src_cx,  unsigned char src_cy,
                 unsigned char dest_cx, unsigned char dest_cy,
                 unsigned char cells_w, unsigned char cells_h)
{
    unsigned char cx, cy;
    unsigned int src_cell;
    unsigned int dest_cell;

    for (cy = 0; cy < cells_h; cy++) {
        if (dest_cy + cy >= 25) break;
        if (src_cy  + cy >= 25) break;
        for (cx = 0; cx < cells_w; cx++) {
            if (dest_cx + cx >= 40) break;
            if (src_cx  + cx >= 40) break;
            src_cell  = (unsigned int)(src_cy  + cy) * 40 + (src_cx  + cx);
            dest_cell = (unsigned int)(dest_cy + cy) * 40 + (dest_cx + cx);
            memcpy((unsigned char*)0x6000 + dest_cell * 8,
                   data + src_cell * 8, 8);
            *((unsigned char*)0x4400 + dest_cell) = *(data + 8000 + src_cell);
        }
    }
}


void FILL_SCREEN_TILE(const unsigned char* data,
                      unsigned char cells_w, unsigned char cells_h,
                      unsigned char fg,      unsigned char bg)
{
   static unsigned char  tx, ty;
   static unsigned char  tile_cx, tile_cy;
   static unsigned int   tile_cell;           // ← POPRAWKA: int, nie char
   static  unsigned int   color_offset;        // ← NOWE: wsparcie dla AUTO
   static unsigned char  orig_color, color;
   static unsigned char* pix_ptr;
   static unsigned char* col_ptr;

    color_offset = (unsigned int)cells_w * cells_h * 8;  // ← NOWE

    pix_ptr  = (unsigned char*)0x6000;
    col_ptr  = (unsigned char*)0x4400;

    for (ty = 0; ty < 25; ty++) {
        tile_cy = ty % cells_h;
        for (tx = 0; tx < 40; tx++) {
            tile_cx  = tx % cells_w;
            tile_cell = (unsigned int)tile_cy * cells_w + tile_cx;  // ← int!

            memcpy(pix_ptr, data + tile_cell * 8, 8);

            // Obsługa TILE_COLOR_AUTO — jak w DRAW_TILE
            orig_color = *(data + color_offset + tile_cell);

            if (fg == TILE_COLOR_AUTO && bg == TILE_COLOR_AUTO) {
                color = orig_color;
            } else if (fg == TILE_COLOR_AUTO) {
                color = (orig_color & 0xF0) | (bg & 0x0F);
            } else if (bg == TILE_COLOR_AUTO) {
                color = (fg << 4) | (orig_color & 0x0F);
            } else {
                color = (fg << 4) | (bg & 0x0F);
            }

            *col_ptr = color;

            pix_ptr += 8;
            col_ptr += 1;
        }
    }
}


// ============================================================
//  DRAW_TILE(data, dest_cx, dest_cy, cells_w, cells_h, fg, bg)
//
//  Rysuje maly kafel na ekranie z mozliwoscia nadpisania kolorow.
//
//  data     = tablica kafla z konwertera (piksele + kolory)
//  dest_cx  = kolumna docelowa (0-39, w komorkach 8px)
//  dest_cy  = wiersz docelowy  (0-24, w komorkach 8px)
//  cells_w  = szerokosc kafla w komorkach
//  cells_h  = wysokosc kafla w komorkach
//  fg       = kolor pikseli zapalonych (0-15), lub TILE_COLOR_AUTO
//  bg       = kolor pikseli zgaszonych (0-15), lub TILE_COLOR_AUTO
//
//  Jezeli fg lub bg = TILE_COLOR_AUTO (255)
//  -> uzywa oryginalnych kolorow z danych kafla
//
//  Przyklady:
//    DRAW_TILE(grass, 4, 3, 4, 4, TILE_COLOR_AUTO, TILE_COLOR_AUTO); // oryginalne kolory
//    DRAW_TILE(grass, 4, 3, 4, 4, 5, 0);   // zielony na czarnym
//    DRAW_TILE(sand,  8, 3, 4, 4, 7, 9);   // zolty na brazowym
//    DRAW_TILE(rock,  12,3, 4, 4, 11, 0);  // ciemnoszary na czarnym
// ============================================================




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

            *((unsigned char*)0x4400 + dest_cell) = final_color;
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

            *((unsigned char*)0x4400 + dest_cell) = final_color;
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