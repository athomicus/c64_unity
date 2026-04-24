// raster.h — NOWA WERSJA bez pollingu
#ifndef RASTER_H
#define RASTER_H

// Statyczny gradient — wywolaj RAZ po InitHiRes()
// Nie uzywa WAIT_RASTER — nie blokuje CPU
void SetAtmosphere(unsigned char skyColor,
                   unsigned char horizonColor,
                   unsigned char borderColor) {
    // Tlo bitmapy = kolor pikseli "0" w calej bitmapie
    *((unsigned char*)0xD021) = skyColor;
    // Ramka
    *((unsigned char*)0xD020) = borderColor;
}

// Efekt Mordoru — zmien kolory coloram na dolnych wierszach
// Wywolaj RAZ po narysowaniu tilesow
void SetMordorColors(void) {
    unsigned char cx;
    // Wiersze 21-24 (mordor) — nadpisz kolor tla na czerwono-brazowy
    for (cx = 0; cx < 40; cx++) {
        // wiersz 21
        *((unsigned char*)0x4400 + 21*40 + cx) =
            (*((unsigned char*)0x4400 + 21*40 + cx) & 0xF0) | 2; // bg=czerwony
        // wiersz 22
        *((unsigned char*)0x4400 + 22*40 + cx) =
            (*((unsigned char*)0x4400 + 22*40 + cx) & 0xF0) | 9; // bg=brazowy
        // wiersz 23
        *((unsigned char*)0x4400 + 23*40 + cx) =
            (*((unsigned char*)0x4400 + 23*40 + cx) & 0xF0) | 8; // bg=pomaranczowy
    }
}

#endif