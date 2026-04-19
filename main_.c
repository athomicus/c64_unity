#include "unity.h"
#include <conio.h>
#include <time.h>
#include "sprite.h"
#include "sprites_data.h"
#include "bitmap.h"
//#include "ekran1.h"
#include "mapka_half.h"
#include "cyfry_hud.h"
//#include "ekran2.h"
#define SPEED 3

// ============================================================
//  Wlacza tryb hires bez kopiowania danych
//  (odpowiednik BITMAP() ale bez memcpy)
// ============================================================
void InitHiRes(void) {
    // Bank VIC = bank 1 (0x4000-0x7FFF)
    *((unsigned char*)0xDD00) = (*((unsigned char*)0xDD00) & 0xFC) | 0x02;

    // Wyczysc bitmape (8000 bajtow pod 0x6000) — caly ekran czarny
    memset((unsigned char*)0x6000, 0x00, 8000);

    // Wyczysc coloram (1000 bajtow pod 0x4400) — wszystko czarne
    memset((unsigned char*)0x4400, 0x00, 1000);

    // Wlacz tryb bitmap hires
    VIC.ctrl1 |=  0x20;   // BMM = 1
    VIC.ctrl2 &= ~0x10;   // MCM = 0
    VIC.addr   =  0x18;   // bitmapa @ 0x6000, coloram @ 0x4400
}

// ============================================================
//  MAIN
// ============================================================
int main(void) {
 	unsigned int  shipX    = 280;        // startowa pozycja X statku
    unsigned char shipY    = 120;        // startowa pozycja Y statku
	unsigned int  playerX    = 280;        // startowa pozycja X statku
    unsigned char playerY    = 120;        // startowa pozycja Y statku
	unsigned int joy;
	clock_t gameClock = clock();    
	clrscr();
    InitSFX();
	LoadMusic("dupa.mus");
    PlayMusic();
	
 // Zaladuj muzyke (musi byc w projekcie jako asset)
 
	 
   
  InitHiRes(); 
	//BITMAP_COLOR(0,12);
	//BITMAP(ekran1);
   
	//320x200 ale od [x+24, y+50, x+344,y+250]
    POKE(0xD020, 0);  // ramka: czarna
    POKE(0xD021, 0);  // tlo:   czarne             
	DRAW_TILE(ekran1, 0, 0, 20, 25, C64_GREEN, C64_BLACK);
	DRAW_TILE_FLIP_H(ekran1, 20, 0, 20, 25, C64_GREEN, C64_BLACK);
                   
	//DRAW_TILE(ekran1, 20, 0, 20, 25, C64_GREEN, C64_BLACK);
	//DRAW_TILE(teren,  0,  0,2,2, C64_GREEN, C64_BLACK);
	//DRAW_TILE(ekran1,  0,  0,20,25, C64_GREEN, C64_BLACK);//40x25
	 
	
	// --- zmiana koloru bitmapy ---



	
	
	
	SPRITE(0, spritePlayer,  60,  80,  1);  // bialy
    SPRITE(1, spriteEnemy, 120,  80,  2);  // czerwony
	SPRITE(2, spriteEnemy, 50,  50,  7);  // czerwony

    
DrawNumber(77,2,1,C64_WHITE,  C64_BLACK);
	while (1) {
		
		  
	
		  // Takt czasowy — ruch co 1 tyknięć zegara
        if (clock() > gameClock 	)
		{
           gameClock = clock();

			//######### SPRITE 01
            // Przesuń statek w lewo
            if (shipX > 24)
                shipX -= 1;        // prędkość: zmień na 2 lub 3 dla szybszego ruchu
            else
                shipX = 320;       // gdy dojedzie do lewej — teleport na prawą
            // Aktualizuj tylko pozycję (bez kopiowania danych sprite'a!)
            SPRITE_MOVE(1, shipX, shipY);
			
			
		 //######### SPRITE 02
		 // Odczyt joysticka przez 8bit-Unity (port 2)
           joy = ~GetJoy(0);//^ 0x1F;
		   
			
			//joy = GetJoy(1);
			 // Ruch
            if (joy & JOY_UP)     { if (playerY > 50)  playerY -= SPEED; PlaySFX(SFX_GUN, 180, 255, 0); }
            if (joy & JOY_DOWN)   { if (playerY < 220) playerY += SPEED; PlaySFX(SFX_GUN, 180, 255, 0);}
			if (joy & JOY_LEFT)   { if (playerX > 24)  playerX -= SPEED; }
            if (joy & JOY_RIGHT)  { if (playerX < 320) playerX += SPEED; }
			SPRITE_MOVE(0, playerX, playerY);	
			 
		 

        }
		
		
	}
 
    return 0;
}
