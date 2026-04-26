#include "unity.h"
#include <conio.h>
#include <time.h>
#include "sprite.h"
#include "sprites_data.h"
#include "bitmap.h"
#include "font_hud.h"
 
// Hitbox sprite 24x21 z insetem 4px:
#define HB_L  4   // lewy  narożnik hitboxu (od lewej krawędzi sprite'a)
#define HB_R 19   // prawy narożnik hitboxu (24 - 5)
#define HB_T  4   // górny narożnik
#define HB_B 16   // dolny narożnik (21 - 5)

#define SPEED 3
#define ANIM_SPEED 9
#define SPRITE_ENABLE(slot)   *((unsigned char*)0xD015) |=  (1 << (slot))
#define SPRITE_DISABLE(slot)  *((unsigned char*)0xD015) &= ~(1 << (slot))
unsigned char points = 0;
unsigned char ringCollected = 0;

void RestartGame(void) {
    // Skok bezpośrednio na adres startowy programu
    __asm__("jmp $180D");   // adres jmp z buildu — masz go w loaderze!
}
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
    memset((unsigned char*)0x4800, 0x00, 1000);

    // Wlacz tryb bitmap hires
    VIC.ctrl1 |=  0x20;   // BMM = 1
    VIC.ctrl2 &= ~0x10;   // MCM = 0
    
    //zmiana
    //VIC.addr   =  0x18;   // bitmapa @ 0x6000, coloram @ 0x4400
    VIC.addr = 0x28;
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

unsigned char CanWalk(unsigned int px, unsigned int py)
{
    unsigned char tx, ty;
    if (px < 24 || px > 343) return 0;
    if (py < 90 || py > 249) return 0;
    tx = (unsigned char)((px - 24) / 16);
    ty = (unsigned char)((py - 90) / 16);
    if (tx >= 20 || ty >= 10) return 0;
    return (mapa[ty][tx] == 0) ? 1 : 0;
}

void DrawMap(void) { // po Y offset 5 kafli w dol bo napis mordoru: 5*8 spikseli
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
	unsigned int  playerX    = 100;        // startowa pozycja X statku
    unsigned char playerY    = 110;        // startowa pozycja Y statku
    unsigned int  newX, newY;
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

 

//mordor speak language
 DRAW_TILE(n01, 0, 0, 4, 4, 2,0);
 DRAW_TILE(n02, 4, 0, 4, 4, 2,0 );
 DRAW_TILE(n03, 8, 0, 4, 4, 2,0 );
 DRAW_TILE(n04, 12, 0, 4, 4, 2,0 );
 DRAW_TILE(n05, 16, 0, 4, 4, 2,0 );
 DRAW_TILE(n02, 20, 0, 4, 4, 2,0 );
 DRAW_TILE(n03, 24, 0, 4, 4, 2,0 );
 DRAW_TILE(n05, 28, 0, 4, 4, 2,0 );
 //DRAW_TILE(n04, 32, 0, 4, 4, 2,0 );
// DRAW_TILE(n05, 36, 0, 4, 4, 2,0 );

//DrawChar('A', 5, 2, C64_WHITE, C64_BLACK);
    DrawText("One Ring to rule them all", 6, 7, C64_WHITE, C64_BLACK);    
    DrawText("One Ring to find them", 9, 9, C64_WHITE, C64_BLACK);    
    DrawText("One Ring to bring them all", 6, 11, C64_WHITE, C64_BLACK);    
    DrawText("and in the darkness bind them",5,13, C64_WHITE, C64_BLACK); 

//draw how to play
DrawText("Find the other Rings avoid enemies",3, 18, C64_DARKGRAY , C64_BLACK);    
DrawText("bring the Rings to the hobbits hut", 2, 19, C64_DARKGRAY , C64_BLACK);    
DrawText("Use the ONE RING ",10,21, C64_DARKGRAY , C64_BLACK);    
DrawText("press fire button to become invisible",0,22, C64_DARKGRAY , C64_BLACK);    
DrawText("but it slowly kills you", 7, 23, C64_DARKGRAY , C64_BLACK); 
DrawText("Getting caught means instant death", 2, 24, C64_DARKGRAY , C64_BLACK);   


     // Czekaj na fire (joystick port 2)

while (1) {
    joy = ~GetJoy(0);
    if (joy & JOY_BTN1) break;
}


InitHiRes();
    // narysuj  wszedzie trawe:
// FILL_SCREEN_TILE(grass,2,2, 11, 0);
   //mordor speak language
 
 DRAW_TILE(n01, 0, 0, 4, 4, 11,0);
 DRAW_TILE(n02, 4, 0, 4, 4, 11,0 );
 DRAW_TILE(n03, 8, 0, 4, 4, 11,0 );
 DRAW_TILE(n04, 12, 0, 4, 4, 11,0 );
 DRAW_TILE(n05, 16, 0, 4, 4, 11,0 );
 DRAW_TILE(n02, 20, 0, 4, 4, 11,0 );
 DRAW_TILE(n03, 24, 0, 4, 4, 11,0 );
 DRAW_TILE(n05, 28, 0, 4, 4, 11,0 );


   
 DrawMap();

//HObbit cottage
    DRAW_TILE(hobbit_small, 32, 20, 4, 4, 9,0 ); //chata 
    //DRAW_TILE(hobit2, 36, 20, 4, 4, 9,0 ); //chata 2
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

  
   


//

    //DRAW_TILE(ekran1, 0, 0, 20, 25, C64_GREEN, C64_BLACK);
	//DRAW_TILE_FLIP_H(ekran1, 20, 0, 20, 25, C64_GREEN, C64_BLACK);
                   
	//DRAW_TILE(ekran1, 20, 0, 20, 25, C64_GREEN, C64_BLACK);
	//DRAW_TILE(teren,  0,  0,2,2, C64_GREEN, C64_BLACK);
	//DRAW_TILE(ekran1,  0,  0,20,25, C64_GREEN, C64_BLACK);//40x25
	 
	
	// --- zmiana koloru bitmapy ---



	
	
	
	SPRITE(0, spritePlayer,  playerX,  playerY,  1);  // bialy
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
          
             //unsigned char can_move =  CanWalk()
            if (joy & JOY_UP)
            { 
                newX = playerX;
                newY = playerY - SPEED;      
                //Sprite 24x21 px jego srodek: x=24-12=12, y=21-10=~10
                // mapa po Y zaczyna sie Y - 50(vic_offset)+10(sprite center) -40(napis mordoru) = -80
              if (CanWalk(newX+HB_L, newY+HB_T) && CanWalk(newX+HB_R, newY+HB_T))    playerY = (unsigned char)newY;        
            }
            
            
            
            if (joy & JOY_DOWN)   
            {
                newX = playerX;
                newY =  playerY + SPEED;
                if (CanWalk(newX+HB_L, newY+HB_B) && CanWalk(newX+HB_R, newY+HB_B))    playerY = (unsigned char)newY;
            }
			
            if (joy & JOY_LEFT)
            { 
                newX = playerX - SPEED;
                newY = playerY;
                if (CanWalk(newX+HB_L, newY+HB_T) && CanWalk(newX+HB_L, newY+HB_B))      playerX = newX;
            }    


            if (joy & JOY_RIGHT)
            { 
               newX = playerX + SPEED;
               newY = playerY;
               if (CanWalk(newX+HB_R, newY+HB_T) && CanWalk(newX+HB_R, newY+HB_B))         playerX = newX;
            
            }
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

