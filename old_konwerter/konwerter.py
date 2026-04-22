#!/usr/bin/env python3
# konwerter.py
# Uzycie: python3 konwerter.py obrazek.png ekran1.h
#
# Wymaga: pip install Pillow
#
# Obrazek MUSI byc 320x200 pikseli
# Kazda komorka 8x8 moze miec max 2 kolory (ograniczenie C64 hires)
# Skrypt automatycznie dopasuje kolory do palety C64

import sys
from PIL import Image

# Paleta C64 (RGB)
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

def nearest_c64_color(r, g, b):
    """Znajdz najblizszy kolor C64 dla piksela RGB"""
    best = 0
    best_dist = float("inf")
    for i, (cr, cg, cb) in enumerate(C64_PALETTE):
        dist = (r-cr)**2 + (g-cg)**2 + (b-cb)**2
        if dist < best_dist:
            best_dist = dist
            best = i
    return best

def convert(input_path, output_path):
    img = Image.open(input_path).convert("RGB")

    if img.size != (320, 200):
        print(f"Skaluje z {img.size} do 320x200...")
        img = img.resize((320, 200), Image.LANCZOS)

    pixels = list(img.getdata())

    bitmap  = bytearray(8000)   # piksele
    coloram = bytearray(1000)   # kolory komorek

    for cell_y in range(25):       # 25 wierszy komorek
        for cell_x in range(40):   # 40 kolumn komorek

            # Zbierz wszystkie piksele tej komorki 8x8
            cell_colors = {}
            for py in range(8):
                for px in range(8):
                    x = cell_x * 8 + px
                    y = cell_y * 8 + py
                    r, g, b = pixels[y * 320 + x]
                    c = nearest_c64_color(r, g, b)
                    cell_colors[c] = cell_colors.get(c, 0) + 1

            # Wybierz 2 najczestsze kolory: fg (wiecej pikseli) i bg (mniej)
            sorted_colors = sorted(cell_colors.items(), key=lambda x: -x[1])
            fg = sorted_colors[0][0]
            bg = sorted_colors[1][0] if len(sorted_colors) > 1 else 0

            # Zapisz bajt koloru
            cell_idx = cell_y * 40 + cell_x
            coloram[cell_idx] = (fg << 4) | (bg & 0x0F)

            # Zapisz 8 bajtow pikseli tej komorki
            for py in range(8):
                byte = 0
                for px in range(8):
                    x = cell_x * 8 + px
                    y = cell_y * 8 + py
                    r, g, b = pixels[y * 320 + x]
                    c = nearest_c64_color(r, g, b)
                    # jesli piksel blizszy fg niz bg -> bit = 1
                    dist_fg = sum((a-b_)**2 for a,b_ in zip((r,g,b), C64_PALETTE[fg]))
                    dist_bg = sum((a-b_)**2 for a,b_ in zip((r,g,b), C64_PALETTE[bg]))
                    if dist_fg <= dist_bg:
                        byte |= (1 << (7 - px))
                bitmap[cell_idx * 8 + py] = byte

    # Polacz bitmape + coloram = 9000 bajtow
    data = bitmap + coloram

    # Zapisz jako plik .h
    with open(output_path, "w") as f:
        f.write("#ifndef EKRAN1_H\n")
        f.write("#define EKRAN1_H\n\n")
        f.write("static const unsigned char ekran1[9000] = {\n")
        for i in range(0, 9000, 16):
            chunk = data[i:i+16]
            hex_str = ", ".join(f"0x{b:02X}" for b in chunk)
            f.write(f"    {hex_str},\n")
        f.write("};\n\n")
        f.write("#endif\n")

    print(f"Zapisano: {output_path}  ({len(data)} bajtow)")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Uzycie: python3 konwerter.py obrazek.png ekran1.h")
        sys.exit(1)
    convert(sys.argv[1], sys.argv[2])
