#include "unity.h"
#include <conio.h>
#include <time.h>
#include "sprite.h"
#include "sprites_data.h"
#include "bitmap.h"
#include "ekran1.h"
#include "cyfry_hud.h"
//#include "ekran2.h"
#define SPEED 3

   

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
 
	 
   
    InitBitmap();
	BITMAP(mapa1);	
	BITMAP_COLOR(0,12);
	
	DRAW_TILE(teren,  0,  0,2,2, C64_GREEN, C64_BLACK);
	 
	
	// --- zmiana koloru bitmapy ---



	
	//320x200 ale od [x+24, y+50, x+344,y+250]
    POKE(0xD020, 0);  // ramka: czarna
    POKE(0xD021, 0);  // tlo:   czarne
	
	
	SPRITE(0, sprite0Data,  60,  80,  1);  // bialy
    SPRITE(1, sprite1Data, 120,  80,  2);  // czerwony
	SPRITE(2, sprite1Data, 50,  50,  7);  // czerwony

    
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
