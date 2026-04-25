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
//#include "raster.h"
//#include "ekran2.h"
#define SPEED 3
#define ANIM_SPEED 9
#define SPRITE_ENABLE(slot)   *((unsigned char*)0xD015) |=  (1 << (slot))
#define SPRITE_DISABLE(slot)  *((unsigned char*)0xD015) &= ~(1 << (slot))
unsigned char points = 0;
unsigned char ringCollected = 0;
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
 

void CheckSpriteCollisions(void)
 {
    unsigned char col = *((unsigned char*)0xD01E);
    //$D01E — sprite–sprite collision
    // Gracz (slot 0) uderzyl w kogos?
    // col & 0x01 = gracz bral udzial
    // col & 0x02 = slot 1 (enemy) bral udzial
    // Oba bity = kolizja GRACZA z WROGIEM

    if ((col & 0x01) && (col & 0x08)) {
        // gracz zbiera pierscien
        if(!ringCollected )
        {
            points += 10;
            DrawNumber(points, 1, 24, C64_WHITE, C64_BLACK);
            SPRITE_DISABLE(3);
            ringCollected=1;
        }
         
    }
/*

    Slot	Bit	Maska	Proponowane użycie
0	bit 0	0x01	PLAYER (Hobbit)
1	bit 1	0x02	NASGUL
2	bit 2	0x04	ORK
3	bit 3	0x08	RING
4	bit 4	0x10	
5	bit 5	0x20	
6	bit 6	0x40	
7	bit 7	0x80	
*/
}


void DrawMap(void) {
    unsigned char x, y;
    
     for (y = 0; y < 10; y++) 
     {
        for (x = 0; x < 20; x++) 
        {
           if (mapa[y][x] == 1)
           {
             DRAW_TILE(tree, x*2, y*2+5, 2, 2, C64_BLACK,C64_GREEN);     
           }
           if (mapa[y][x] == 2)
           {
             DRAW_TILE(tree2, x*2, y*2+5, 2, 2, C64_BLACK,C64_GREEN);     
           }
           if (mapa[y][x] == 3)
           {
             DRAW_TILE(pien, x*2, y*2+5, 2, 2,  C64_BROWN,C64_BLACK);     
           }           
           if (mapa[y][x] == 4)
           {
             DRAW_TILE(roslina, x*2, y*2+5, 2, 2,  C64_GREEN,C64_BLACK);     
           }
           
           if (mapa[y][x] == 5)
           {
             DRAW_TILE(stone, x*2, y*2+5, 2, 2,  C64_LIGHTGRAY,C64_BLACK);     
           }

           if (mapa[y][x] == 6)
           {
             DRAW_TILE(tree_left, x*2, y*2+5, 2, 2, C64_GREEN, C64_BLACK);     
           }
          
           if (mapa[y][x] == 7)
           {
             DRAW_TILE(tree_center,x*2, y*2+5, 2, 2, C64_GREEN, C64_BLACK);     
           }
          
           if (mapa[y][x] == 8)
           {
             DRAW_TILE(tree_right, x*2, y*2+5, 2, 2,  C64_GREEN, C64_BLACK);     
           }
          
           if (mapa[y][x] == 9)
           {
             DRAW_TILE(tree_up, x*2, y*2+5, 2, 2, C64_GREEN, C64_BLACK);     
           }

           if (mapa[y][x] == 13)
           {
             DRAW_TILE(plant_up, x*2, y*2+5, 2, 2, C64_GREEN, C64_BLACK);     
           }
           
           if (mapa[y][x] == 10)
           {
             DRAW_TILE(plant_left, x*2, y*2+5, 2, 2, C64_GREEN, C64_BLACK);     
           }

           
           if (mapa[y][x] == 11)
           {
             DRAW_TILE(plant_center, x*2, y*2+5, 2, 2, C64_GREEN, C64_BLACK);     
           }

           
           if (mapa[y][x] == 12)
           {
             DRAW_TILE(plant_right, x*2, y*2+5, 2, 2, C64_GREEN, C64_BLACK);     
           }

            if (mapa[y][x] == 14)
           {
             DRAW_TILE(stone_left, x*2, y*2+5, 2, 2, C64_LIGHTGRAY, C64_BLACK);     
           }

           
           if (mapa[y][x] == 15)
           {
             DRAW_TILE(stone_center, x*2, y*2+5, 2, 2, C64_LIGHTGRAY, C64_BLACK);     
           }
           if (mapa[y][x] == 16)
           {
             DRAW_TILE(stone_right, x*2, y*2+5, 2, 2, C64_LIGHTGRAY, C64_BLACK);     
           }
            
        }
    }
}  
// ============================================================
//  MAIN
// ============================================================
int main(void) {
 	unsigned int  shipX    = 280;        // startowa pozycja X statku
    unsigned char shipY    = 120;        // startowa pozycja Y statku
	unsigned int  playerX    = 280;        // startowa pozycja X statku
    unsigned char playerY    = 120;        // startowa pozycja Y statku
    unsigned char animFrame   = 0;  // 0 lub 1
    unsigned char animTimer   = 0;  // licznik klatek
   
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
   
    DrawMap();

//HObbit cottage
    DRAW_TILE(hobit1, 32, 20, 4, 4, 9,0 ); //chata 1
    DRAW_TILE(hobit2, 36, 20, 4, 4, 9,0 ); //chata 2
//

   // DRAW_TILE(tree, 10, 10, 2, 2, C64_BLACK,C64_GREEN);  1
   // DRAW_TILE(tree2, 12, 10, 2, 2, C64_BLACK,C64_GREEN); 2   
    //DRAW_TILE(pien, 14, 10, 2, 2, C64_BROWN,C64_BLACK ); 3
  //  DRAW_TILE(roslina, 16, 10, 2, 2, C64_LIGHTGREEN,C64_BLACK ); 4
  //  DRAW_TILE(stone, 18, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );  5
   // DRAW_TILE(dom1, 20, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );
  //  DRAW_TILE(dom2, 22, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );
   // DRAW_TILE(dom3, 24, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );
   // DRAW_TILE(dom4, 26, 10, 2, 2, C64_LIGHTGRAY  ,C64_BLACK );

  
//DrawChar('A', 5, 2, C64_WHITE, C64_BLACK);
    DrawText("hello world", 2, 1, C64_WHITE, C64_BLACK);    

//mordor speak
 DRAW_TILE(n01, 0, 0, 4, 4, 11,0);
 DRAW_TILE(n02, 4, 0, 4, 4, 11,0 );
 DRAW_TILE(n03, 8, 0, 4, 4, 11,0 );
 DRAW_TILE(n04, 12, 0, 4, 4, 11,0 );
 DRAW_TILE(n05, 16, 0, 4, 4, 11,0 );
 DRAW_TILE(n06, 20, 0, 4, 4, 11,0 );
 DRAW_TILE(n07, 24, 0, 4, 4, 11,0 );
 DRAW_TILE(n08, 28, 0, 4, 4, 11,0 );
 DRAW_TILE(n09, 32, 0, 4, 4, 11,0 );
 DRAW_TILE(n10, 36, 0, 4, 4, 8,0 );


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

    
    DrawNumber(points,2,1,C64_WHITE,  C64_BLACK);
	
  //SetAtmosphere(6, 14, 6);   // niebo=niebieski, ramka=niebieski
//SetMordorColors();
    while (1) {



 //	LoadMusic("dupa.mus");
//   PlayMusic();
	
 // Zaladuj muzyke (musi byc w projekcie jako asset)
		
		  
	
		  // Takt czasowy — ruch co 1 tyknięć zegara
        if (clock() > gameClock 	)
		{
           gameClock = clock();

			//SPRITE DIAMOND
            animTimer++;
            if(animTimer>=ANIM_SPEED && !ringCollected)
            {
                animTimer=0; //wyzeruj timer dla animacji sprite
                animFrame ^= 1;  // przełącz 0->1->0->1
                 if (animFrame == 0) 
                 {
                    SPRITE(3, ring1,  85, 85,  C64_LIGHTBLUE );
                 } 
                 else
                 {
                    SPRITE(3, ring2,  85, 85,  C64_BLUE);
                 }

            }
            ////////////////////////////

            CheckSpriteCollisions();


            //
           
           
           
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

    

    
    */

