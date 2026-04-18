#!/usr/bin/env python3
# konwerter16px.py  — konwertuje obrazek 16x16px do kafla C64 hires
# Uzycie: python3 konwerter16px.py obrazek.png nazwa_kafla
# Wynik:  nazwa_kafla.h  z  static const unsigned char nazwa_kafla[36]
#
# Kafel: 16x16px = 2x2 komorek 8x8
# Format: [32B pikseli][4B kolorow] = 36B razem
# DRAW_TILE(nazwa, dest_cx, dest_cy, 2, 2);
#
# pip install Pillow

import sys
from PIL import Image

CELLS       = 2
TILE_PX     = 16
PIXEL_BYTES = 32
COLOR_BYTES = 4
TOTAL       = 36

C64_PALETTE = [
    (0,   0,   0  ),  # 0  czarny
    (255, 255, 255),  # 1  bialy
    (136, 0,   0  ),  # 2  czerwony
    (170, 255, 238),  # 3  cyan
    (204, 68,  204),  # 4  fioletowy
    (0,   204, 85 ),  # 5  zielony
    (0,   0,   170),  # 6  niebieski
    (238, 238, 119),  # 7  zolty
    (221, 136, 85 ),  # 8  pomaranczowy
    (102, 68,  0  ),  # 9  brazowy
    (255, 119, 119),  # 10 jasno-czerwony
    (51,  51,  51 ),  # 11 ciemnoszary
    (119, 119, 119),  # 12 szary
    (170, 255, 102),  # 13 jasno-zielony
    (0,   136, 255),  # 14 jasno-niebieski
    (187, 187, 187),  # 15 jasnoszary
]

def nearest_c64(r, g, b):
    best, best_dist = 0, float("inf")
    for i, (cr, cg, cb) in enumerate(C64_PALETTE):
        d = (r-cr)**2 + (g-cg)**2 + (b-cb)**2
        if d < best_dist:
            best_dist = d
            best = i
    return best

def convert(input_path, tile_name):
    img = Image.open(input_path).convert("RGB")
    if img.size != (TILE_PX, TILE_PX):
        print(f"Skaluje z {img.size} do {TILE_PX}x{TILE_PX}px...")
        img = img.resize((TILE_PX, TILE_PX), Image.LANCZOS)

    pixels = list(img.getdata())
    pixel_data = bytearray(PIXEL_BYTES)
    color_data = bytearray(COLOR_BYTES)

    for cy in range(CELLS):
        for cx in range(CELLS):
            cell_idx = cy * CELLS + cx
            cell_colors = {}
            for py in range(8):
                for px in range(8):
                    r, g, b = pixels[(cy*8+py) * TILE_PX + (cx*8+px)]
                    c = nearest_c64(r, g, b)
                    cell_colors[c] = cell_colors.get(c, 0) + 1

            sorted_c = sorted(cell_colors.items(), key=lambda x: -x[1])
            fg = sorted_c[0][0]
            bg = sorted_c[1][0] if len(sorted_c) > 1 else 0
            color_data[cell_idx] = (fg << 4) | (bg & 0x0F)

            for py in range(8):
                byte = 0
                for px in range(8):
                    r, g, b = pixels[(cy*8+py) * TILE_PX + (cx*8+px)]
                    df = sum((a-b_)**2 for a,b_ in zip((r,g,b), C64_PALETTE[fg]))
                    db = sum((a-b_)**2 for a,b_ in zip((r,g,b), C64_PALETTE[bg]))
                    if df <= db:
                        byte |= (1 << (7 - px))
                pixel_data[cell_idx * 8 + py] = byte

    data = pixel_data + color_data
    out_path = tile_name + ".h"
    guard = tile_name.upper() + "_H"
    cells_w = 2
    cells_h = 2

    with open(out_path, "w") as f:
        f.write(f"#ifndef {guard}\n")
        f.write(f"#define {guard}\n\n")
        f.write(f"/* Kafel 16x16px (2x2 komorek), rozmiar=36B */\n")
        f.write(f"/* Uzycie: DRAW_TILE({tile_name}, dest_cx, dest_cy, 2, 2); */\n\n")
        f.write(f"static const unsigned char {tile_name}[36] = {{\n")
        f.write("    /* --- piksele (32B) --- */\n")
        for i in range(0, PIXEL_BYTES, 8):
            chunk = data[i:i+8]
            f.write("    " + ", ".join(f"0x{b:02X}" for b in chunk) + ",\n")
        f.write("    /* --- kolory (4B: gorny nibble=fg, dolny=bg) --- */\n")
        for i in range(PIXEL_BYTES, TOTAL, 8):
            chunk = data[i:min(i+8, TOTAL)]
            f.write("    " + ", ".join(f"0x{b:02X}" for b in chunk) + ",\n")
        f.write("};\n\n")
        f.write(f"#endif\n")

    print(f"Zapisano: {out_path}  ({TOTAL} bajtow)")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Uzycie: python3 konwerter16px.py obrazek.png nazwa_kafla")
        print(f"Przyklad: python3 konwerter16px.py grass.png grass")
        print(f"Wynik:    grass.h  z  DRAW_TILE(grass, x, y, 2, 2);")
        sys.exit(1)
    convert(sys.argv[1], sys.argv[2])
