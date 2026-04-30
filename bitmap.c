// bitmap.c
#include "bitmap.h"
#include <string.h>
#include <c64.h>
#define COPY8(dst, src) \
    (dst)[0]=(src)[0]; (dst)[1]=(src)[1]; (dst)[2]=(src)[2]; (dst)[3]=(src)[3]; \
    (dst)[4]=(src)[4]; (dst)[5]=(src)[5]; (dst)[6]=(src)[6]; (dst)[7]=(src)[7];

// ============================================================
void DRAW_TILE(const unsigned char* data,
               unsigned char dest_cx, unsigned char dest_cy,
               unsigned char cells_w, unsigned char cells_h,
               unsigned char fg, unsigned char bg)
{
    unsigned char cx, cy;
    unsigned char orig_color, final_color;
    unsigned int  color_offset, tile_cell, dest_cell;
    unsigned char* dst;
    const unsigned char* src;

    color_offset = (unsigned int)cells_w * cells_h * 8;

    for (cy = 0; cy < cells_h; cy++) {
        if (dest_cy + cy >= 25) break;
        for (cx = 0; cx < cells_w; cx++) {
            if (dest_cx + cx >= 40) break;

            tile_cell = (unsigned int)cy * cells_w + cx;
            dest_cell = (unsigned int)(dest_cy + cy) * 40 + (dest_cx + cx);

            dst = (unsigned char*)0x6000 + dest_cell * 8;
            src = data + tile_cell * 8;
            COPY8(dst, src);

            orig_color = *(data + color_offset + tile_cell);

            if (fg == TILE_COLOR_AUTO && bg == TILE_COLOR_AUTO) {
                final_color = orig_color;
            } else if (fg == TILE_COLOR_AUTO) {
                final_color = (orig_color & 0xF0) | (bg & 0x0F);
            } else if (bg == TILE_COLOR_AUTO) {
                final_color = ((fg & 0x0F) << 4) | (orig_color & 0x0F);
            } else {
                final_color = ((fg & 0x0F) << 4) | (bg & 0x0F);
            }

            *((unsigned char*)0x4800 + dest_cell) = final_color;
        }
    }
}

// ============================================================
void DRAW_TILE_FLIP_H(const unsigned char* data,
                      unsigned char dest_cx, unsigned char dest_cy,
                      unsigned char cells_w, unsigned char cells_h,
                      unsigned char fg, unsigned char bg)
{
    unsigned char cx, cy, py;
    unsigned char orig_color, final_color;
    unsigned char src_byte, flipped_byte;
    unsigned int  color_offset, src_cell, dest_cell;

    color_offset = (unsigned int)cells_w * cells_h * 8;

    for (cy = 0; cy < cells_h; cy++) {
        if (dest_cy + cy >= 25) break;
        for (cx = 0; cx < cells_w; cx++) {
            if (dest_cx + cx >= 40) break;

            src_cell  = (unsigned int)cy * cells_w + (cells_w - 1 - cx);
            dest_cell = (unsigned int)(dest_cy + cy) * 40 + (dest_cx + cx);

            for (py = 0; py < 8; py++) {
                src_byte = *(data + src_cell * 8 + py);
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

            orig_color = *(data + color_offset + src_cell);

            if (fg == TILE_COLOR_AUTO && bg == TILE_COLOR_AUTO) {
                final_color = ((orig_color & 0x0F) << 4) | ((orig_color & 0xF0) >> 4);
            } else if (fg == TILE_COLOR_AUTO) {
                final_color = (orig_color & 0xF0) | (bg & 0x0F);
            } else if (bg == TILE_COLOR_AUTO) {
                final_color = ((fg & 0x0F) << 4) | (orig_color & 0x0F);
            } else {
                final_color = ((fg & 0x0F) << 4) | (bg & 0x0F);
            }

            *((unsigned char*)0x4800 + dest_cell) = final_color;
        }
    }
}