#include "unity.h"
#include <conio.h>
#include <time.h>
#include "sprite.h"
#include "sprites_data.h"
#include "bitmap.h"
//#include "ekran1.h"
#include "mapka_half.h"
#include "cyfry_hud.h"
#include "font_hud.h"
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
 
	 
   
  InitHiRes(); 
	//BITMAP_COLOR(0,0);
	//BITMAP(mapa);
   
	//320x200 ale od [x+24, y+50, x+344,y+250]
    POKE(0xD020, 0);  // ramka: czarna
    POKE(0xD021, 0);  // tlo:   czarne             
// narysuj  wszedzie trawe:
// FILL_SCREEN_TILE(grass,2,2, 11, 0);
    

    DRAW_TILE(tree, 10, 10, 2, 2, C64_BLACK,C64_GREEN);
    DRAW_TILE(tree2, 12, 10, 2, 2, C64_BLACK,C64_GREEN);
    DRAW_TILE(pien, 14, 10, 2, 2, C64_BROWN,C64_BLACK );
    DRAW_TILE(roslina, 16, 10, 2, 2, C64_LIGHTGREEN,C64_BLACK );
    DRAW_TILE(stone, 18, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );
    DRAW_TILE(dom1, 20, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );
    DRAW_TILE(dom2, 22, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );
    DRAW_TILE(dom3, 24, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );
    DRAW_TILE(dom4, 26, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );

  
//DrawChar('A', 5, 2, C64_WHITE, C64_BLACK);
    DrawText("hello world", 2, 1, C64_WHITE, C64_BLACK);    

//mordor
 DRAW_TILE(n01, 0, 21, 4, 4, 11,0);
 DRAW_TILE(n02, 4, 21, 4, 4, 11,0 );
 DRAW_TILE(n03, 8, 21, 4, 4, 11,0 );
 DRAW_TILE(n04, 12, 21, 4, 4, 11,0 );
 DRAW_TILE(n05, 16, 21, 4, 4, 11,0 );
 DRAW_TILE(n06, 20, 21, 4, 4, 11,0 );
 DRAW_TILE(n07, 24, 21, 4, 4, 11,0 );
 DRAW_TILE(n08, 28, 21, 4, 4, 11,0 );
 DRAW_TILE(n09, 32, 21, 4, 4, 11,0 );
 DRAW_TILE(n10, 36, 21, 4, 4, 8,0 );

DRAW_TILE(hobit1, 20, 1, 4, 4, 9,0 );
 DRAW_TILE(hobit2, 24, 1, 4, 4, 9,0 );
//

    //DRAW_TILE(ekran1, 0, 0, 20, 25, C64_GREEN, C64_BLACK);
	//DRAW_TILE_FLIP_H(ekran1, 20, 0, 20, 25, C64_GREEN, C64_BLACK);
                   
	//DRAW_TILE(ekran1, 20, 0, 20, 25, C64_GREEN, C64_BLACK);
	//DRAW_TILE(teren,  0,  0,2,2, C64_GREEN, C64_BLACK);
	//DRAW_TILE(ekran1,  0,  0,20,25, C64_GREEN, C64_BLACK);//40x25
	 
	
	// --- zmiana koloru bitmapy ---



	
	
	
	SPRITE(0, spritePlayer,  60,  80,  1);  // bialy
    SPRITE(1, spriteEnemy, 120,  80,  2);  // czerwony
	SPRITE(2, spriteOrc, 50,  50,  5);  // zielony

    
DrawNumber(77,2,1,C64_WHITE,  C64_BLACK);
	while (1) {


 //	LoadMusic("dupa.mus");
//   PlayMusic();
	
 // Zaladuj muzyke (musi byc w projekcie jako asset)
		
		  
	
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



//$D01E — sprite–sprite collision
// Kazdy bit = jeden sprite (bit 0 = sprite 0, bit 1 = sprite 1 itd.)
// Jesli bit jest ustawiony = ten sprite bral udzial w kolizji
//unsigned char col = *((unsigned char*)0xD01E);

//if (col & 0x01) { /* sprite 0 (gracz) zderzyl sie z czyms */ }
//if (col & 0x02) { /* sprite 1 zderzyl sie z czyms */ }
/*
void CheckSpriteCollisions(void) {
    unsigned char col = *((unsigned char*)0xD01E);

    // Gracz (slot 0) uderzyl w kogos?
    // col & 0x01 = gracz bral udzial
    // col & 0x02 = slot 1 (enemy) bral udzial
    // Oba bity = kolizja GRACZA z WROGIEM

    if ((col & 0x01) && (col & 0x02)) {
        // gracz <-> enemy1
        playerHP -= 10;
        DrawNumber(playerHP, 1, 24, C64_WHITE, C64_BLACK);
    }

    if ((col & 0x01) && (col & 0x04)) {
        // gracz <-> enemy2 (slot 2)
        playerHP -= 10;
    }

    if (ringActive) {
        // Hobbit niewidzialny — ignoruj kolizje z wrogami
        return;
    }

    
    Slot	Bit	Maska	Proponowane użycie
0	bit 0	0x01	Gracz (Hobbit)
1	bit 1	0x02	Enemy 1
2	bit 2	0x04	Enemy 2
3	bit 3	0x08	Ork
4	bit 4	0x10	Enemy 4 / Boss
5	bit 5	0x20	Pocisk / strzała
6	bit 6	0x40	Skarb latający / NPC
7	bit 7	0x80	Pocisk wroga
    
    */

