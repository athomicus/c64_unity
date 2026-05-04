#include <c64.h>
#include "unity.h"
#include <time.h>
#include "sprite.h"
#include "sprites_data.h"
#include "bitmap.h"
#include "font_hud.h"
#include "sfx.h"

/* ============================================================
   STALE
   ============================================================ */
#define PATCH_END    255
#define HB_L           6
#define HB_R          10
#define HB_T           6
#define HB_B          10
#define COTTAGE_X1   280
#define COTTAGE_Y1   210
#define COTTAGE_X2   312
#define COTTAGE_Y2   242
#define SPEED          2
#define ANIM_SPEED     9
 

#define ORC_ANIM_SPEED  8   // im mniejsza liczba, tym szybsza animacja

#define SPRITE_ENABLE(slot)  *((unsigned char*)0xD015) |=  (1 << (slot))
#define SPRITE_DISABLE(slot) *((unsigned char*)0xD015) &= ~(1 << (slot))

/* ============================================================
   BUFOR MAPY
   ============================================================ */
static unsigned char current_map[10][20];

/* ============================================================
   FORWARD DECLARATIONS
   ============================================================ */
void LoadAndDrawMap(unsigned char prevLevel);
void SpawnRing(void);
void BuildMap(const unsigned char patch[][3]);

/* ============================================================
   ZMIENNE GLOBALNE
   ============================================================ */
unsigned int  points        = 0;
unsigned char player_steps  = 0;
unsigned int  distance      = 0;
unsigned char ringCollected = 0;
unsigned int  playerX       = 100;
unsigned char playerY       = 110;
unsigned char level_map     = 1;
unsigned int  joy;

unsigned int  orc1X         = 30;
unsigned char orc1Y         = 80;

unsigned int  orc2X         = 280;
unsigned char orc2Y         = 80;

unsigned int  orc3X         = 40;
unsigned char orc3Y         = 200;

unsigned int  orc4X         = 280;
unsigned char orc4Y         = 200;

       


unsigned int  nasgul1X         = 320;
unsigned char nasgul1Y         = 120;
unsigned char previousLevel = 0;
unsigned char needRestart   = 0;
unsigned char nasgul1Dir = 0;
unsigned int  ring_power     = 200;  /* budżet — maleje gdy trzyma fire */
unsigned char ringActive     = 0;    /* 1 = pierscien wlaczony teraz */
unsigned char hudSeg = 0;
// unsigned char orcAnimFrame = 0;
unsigned char orcAnimTimer = 0;

static const unsigned int  ring_pos_x[8] = {60, 56, 200, 260, 290, 309, 267, 141};
static const unsigned char ring_pos_y[8] = {128,200, 180, 120, 120, 167, 112, 146};

unsigned char ring_slot  = 0;
unsigned char ring_map   = 1;
unsigned char first_spawn = 1;




/* ============================================================
   LOSOWOŚĆ (SID $D41B)
   ============================================================ */
void InitRandom(void)
{
    *((unsigned char*)0xD40E) = 0xFF;
    *((unsigned char*)0xD40F) = 0xFF;
    *((unsigned char*)0xD412) = 0x80;
}

unsigned char GetRandom(void)
{
    return *((unsigned char*)0xD41B);
}

/* ============================================================
   INICJALIZACJA HIRES
   ============================================================ */
void InitHiRes(void)
{
    *((unsigned char*)0xDD00) = (*((unsigned char*)0xDD00) & 0xFC) | 0x02;
    memset((unsigned char*)0x6000, 0x00, 8000);
    memset((unsigned char*)0x4800, 0x00, 1000);
    VIC.ctrl1 |=  0x20;
    VIC.ctrl2 &= ~0x10;
    VIC.addr   =  0x28;
}

/*

unsigned int AbsDiff(unsigned int a, unsigned int b)
{
       if (a > b) return a - b;
       return b - a;
}


 */
 

//unsigned int AbsDistance(unsigned int a, unsigned int b)//
//{
 //      if (a > b) return a - b;
 //      return b - a;
//}
void MoveOrcs(void)
{
   // unsigned char rand6 = GetRandom() % 6;
        
        
       /* ścigaj gracza */
    if (playerX > orc1X) orc1X++; else orc1X--;
    if (playerY > orc1Y) orc1Y++; else orc1Y--;
    if (playerX > orc2X) orc2X++; else orc2X--;
    if (playerY > orc2Y) orc2Y++; else orc2Y--;
    if (playerX > orc3X) orc3X++; else orc3X--;
    if (playerY > orc3Y) orc3Y++; else orc3Y--;
    if (playerX > orc4X) orc4X++; else orc4X--;
    if (playerY > orc4Y) orc4Y++; else orc4Y--;

    /* unikaj się nawzajem */
    //if (orc1X > orc2X && (orc1X - orc2X) < 20) { orc1X += 5; orc2X -= 5; }
    //if (orc1X < orc2X && (orc2X - orc1X) < 20) { orc1X -= 5; orc2X += 5; }

    //if (orc3X > orc2X && (orc3X - orc2X) < 20) { orc3X += 5; orc2X -= 5; }
    //if (orc3X < orc2X && (orc2X - orc3X) < 20) { orc3X -= 5; orc2X += 5; }

//if (orc1X > orc3X && (orc1X - orc3X) < 20) { orc1X += 5; orc3X -= 5; }
 //   if (orc1X < orc3X && (orc3X - orc1X) < 20) { orc1X -= 5; orc3X += 5; }

    //if (orc1X > orc4X && (orc1X - orc4X) < 20) { orc1X += 5; orc4X -= 5; }
    //if (orc1X < orc4X && (orc4X - orc1X) < 20) { orc1X -= 5; orc4X += 5; }

}


  


/* ============================================================
   HUD
   ============================================================ */
void DrawHUD(void)
{
    unsigned char seg;
    unsigned char i;
    unsigned char col;
    unsigned char d015_save;  // ← dodaj

    seg = (unsigned char)((200 - ring_power) / 25);

    d015_save = *((unsigned char*)0xD015);  // ← zapamiętaj stan
    *((unsigned char*)0xD015) = 0x00;

    for (i = 0; i < 8; i++) {
        col = (i < seg) ? 7 : 11;
        switch(i) {
            case 0: DRAW_TILE(n01,  0, 0, 4, 4, col, 0); break;
            case 1: DRAW_TILE(n02,  4, 0, 4, 4, col, 0); break;
            default: DRAW_TILE(n03, i*4, 0, 4, 4, col, 0); break;
        }
    }

    *((unsigned char*)0xD015) = d015_save;  // ← przywróć zamiast 0x0F
}

    
 

/* ============================================================
   BUFOR MAPY — BuildMap
   ============================================================ */
void BuildMap(const unsigned char patch[][3])
{
    unsigned char i, r, c;
    for (r = 0; r < 10; r++)
        for (c = 0; c < 20; c++)
            current_map[r][c] = mapa_base[r][c];
    for (i = 0; patch[i][0] != PATCH_END; i++)
        current_map[ patch[i][0] ][ patch[i][1] ] = patch[i][2];
}

/* ============================================================
   RYSOWANIE MAPY
   ============================================================ */
void DrawMap(void)
{
    unsigned char x, y;
    for (y = 0; y < 10; y++) {
        for (x = 0; x < 20; x++) {
            switch (current_map[y][x]) {
                case  1: DRAW_TILE(tree,         x*2, y*2+5, 2, 2, C64_BLACK,     C64_GREEN);  break;
                case  2: DRAW_TILE(tree2,        x*2, y*2+5, 2, 2, C64_BLACK,     C64_GREEN);  break;
                case  4: DRAW_TILE(roslina,      x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case  5: DRAW_TILE(stone,        x*2, y*2+5, 2, 2, C64_LIGHTGRAY, C64_BLACK);  break;
                case  6: DRAW_TILE(tree_left,    x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case  7: DRAW_TILE(tree_center,  x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case  8: DRAW_TILE(tree_right,   x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case  9: DRAW_TILE(tree_up,      x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case 10: DRAW_TILE(plant_left,   x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case 11: DRAW_TILE(plant_center, x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case 12: DRAW_TILE(plant_right,  x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case 13: DRAW_TILE(plant_up,     x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case 14: DRAW_TILE(stone_left,   x*2, y*2+5, 2, 2, C64_LIGHTGRAY, C64_BLACK);  break;
                case 15: DRAW_TILE(stone_center, x*2, y*2+5, 2, 2, C64_LIGHTGRAY, C64_BLACK);  break;
                case 16: DRAW_TILE(stone_right,  x*2, y*2+5, 2, 2, C64_LIGHTGRAY, C64_BLACK);  break;
                default: break;
            }
        }
    }
}

/* ============================================================
   ZALADUJ MAPE DO current_map I NARYSUJ
   ============================================================ */
void LoadAndDrawMap(unsigned char prevLevel)
{
    unsigned char r, c;

    switch (level_map) {
        case 1:
            for (r = 0; r < 10; r++)
                for (c = 0; c < 20; c++)
                    current_map[r][c] = mapa1[r][c];
            break;
        case 2: BuildMap(patch2); break;
        case 3: BuildMap(patch3); break;
        case 4: BuildMap(patch4); break;
        default: break;
    }

    DrawMap();

    if (level_map == 1) {
        DRAW_TILE(hobbit_small, 34, 20, 4, 4, 9, 0);
    }

    switch (level_map) {
        case 1:
            if (prevLevel == 2) { playerX =  90; playerY = 200; }
            if (prevLevel == 3) { playerX =  35; playerY = 145; }
            if (prevLevel == 4) { playerX = 300; playerY = 140; }
            break;
        case 2: playerX =  90; playerY = 120; break;
        case 3: playerX = 300; playerY = 145; break;
        case 4: playerX =  35; playerY = 140; break;
        default: playerX =  90; playerY = 200; break;
    }
}

/* ============================================================
   SPAWN PIERSCIENIA
   ============================================================ */
void SpawnRing(void)
{
    if (first_spawn) {
        ring_map   = 1;
        ring_slot  = 0;
        first_spawn = 0;
    } else {
        ring_map  = (GetRandom() & 0x03) + 1;
        ring_slot = GetRandom() & 0x07;
    }

    if (ring_map == level_map) {
        SPRITE(3, ring1, ring_pos_x[ring_slot], ring_pos_y[ring_slot], C64_LIGHTBLUE);
        *((unsigned char*)0xD015) |= 0x08;
    } else {
        SPRITE_DISABLE(3);
    }
}

/* ============================================================
   KOLIZJA Z CHATKA HOBBITA
   ============================================================ */
void CheckCottage(void)
{
    if (!ringCollected) return;
    if ((playerX + HB_R) > COTTAGE_X1 &&
        (playerX + HB_L) < COTTAGE_X2 &&
        (playerY + HB_B) > COTTAGE_Y1 &&
        (playerY + HB_T) < COTTAGE_Y2)
    {
        points += 50;
        DrawNumber(points, 33, 2, C64_WHITE, C64_BLACK);
        ringCollected = 0;
        SpawnRing();
        SFX_Play(SFX_LEVELUP);
    }
}

/* ============================================================
   ZMIANA MAPY
   ============================================================ */
void CheckMapChange(void)
{
    switch (level_map) {
        case 1:
            if (playerX >  72 && playerX < 150 && playerY > 230) level_map = 2;
            if (playerX <  25 && playerY > 130 && playerY < 180) level_map = 3;
            if (playerX > 330 && playerY > 130 && playerY < 160) level_map = 4;
            break;
        case 2:
            if (playerX > 25 && playerX < 150 && playerY < 100) level_map = 1;
            break;
        case 3:
            if (playerX > 330 && playerY > 130 && playerY < 180) level_map = 1;
            break;
        case 4:
            if (playerX < 25 && playerY > 130 && playerY < 180) level_map = 1;
            break;
        default: break;
    }
}

/* ============================================================
   KOLIZJA SPRITE-SPRITE
   ============================================================ */
void CheckSpriteCollisions(void)
{
    unsigned char col = *((unsigned char*)0xD01E);

    if ((col & 0x01) && (col & 0x08)) {
        if (!ringCollected) {
            points += 10;
            DrawNumber(points, 33, 2, C64_WHITE, C64_BLACK);
            SPRITE_DISABLE(3);
            ringCollected = 1;
            SFX_Play(SFX_RING);
        }
    }

    if ((col & 0x01) && (col & 0x02)) {
        needRestart = 1;   /* tylko flaga — glowna petla ogarnie reszte */
    }
}

/* ============================================================
   KOLIZJA Z MAPA
   ============================================================ */
unsigned char CanWalk(unsigned int px, unsigned int py)
{
    unsigned char tx, ty;
    tx = (unsigned char)((px - 24) / 16);
    ty = (unsigned char)((py - 90) / 16);
    if (tx >= 20 || ty >= 10) return 0;
    return (current_map[ty][tx] == 0) ? 1 : 0;
}

/* ============================================================
   MAIN
   ============================================================ */
int main(void)
{
    unsigned char newSeg ;
    unsigned char prevDir; 
    unsigned int   newX;
    signed int     newY;
    unsigned char  animFrame     = 0;
    unsigned char  animTimer     = 0;
    unsigned char  oldLevel;
    unsigned int  total;
     unsigned int  i;
    volatile unsigned char dummy;
    clock_t        gameClock;

    /* --- INIT --- */
    InitRandom();
    InitHiRes();
    POKE(0xD020, 0);
    POKE(0xD021, 0);

    /* --------------------------------------------------------
       EKRAN INTRO
       -------------------------------------------------------- */
    DRAW_TILE(n01,  0, 0, 4, 4, 2, 0);
    DRAW_TILE(n02,  4, 0, 4, 4, 2, 0);
    DRAW_TILE(n03,  8, 0, 4, 4, 2, 0);
    DRAW_TILE(n03, 12, 0, 4, 4, 2, 0);
    DRAW_TILE(n03, 16, 0, 4, 4, 2, 0);
    DRAW_TILE(n03, 20, 0, 4, 4, 2, 0);
    DRAW_TILE(n03, 24, 0, 4, 4, 2, 0);
    DRAW_TILE(n03, 28, 0, 4, 4, 2, 0);
    DrawText("One Ring to rule them all",             6,  7, C64_WHITE,    C64_BLACK);
    DrawText("One Ring to find them",                 9,  9, C64_WHITE,    C64_BLACK);
    DrawText("One Ring to bring them all",            6, 11, C64_WHITE,    C64_BLACK);
    DrawText("and in the darkness bind them",         5, 13, C64_WHITE,    C64_BLACK);
    DrawText("Find the other Rings avoid enemies",    3, 18, C64_DARKGRAY, C64_BLACK);
    DrawText("bring the Rings to the hobbits hut",   2, 19, C64_DARKGRAY, C64_BLACK);
    DrawText("Use the ONE RING",                     10, 21, C64_DARKGRAY, C64_BLACK);
    DrawText("press fire button to become invisible", 0, 22, C64_DARKGRAY, C64_BLACK);
    DrawText("but it slowly kills you",               7, 23, C64_DARKGRAY, C64_BLACK);
    DrawText("Getting caught means instant death",    2, 24, C64_DARKGRAY, C64_BLACK);

    while (1) {
        joy = ~GetJoy(0);
        if (joy & JOY_BTN1) break;
    }

    /* --------------------------------------------------------
       START GRY — ustawienia poczatkowe
       -------------------------------------------------------- */
    level_map     = 1;
    playerX       = 100;
    playerY       = 110;
    nasgul1X         = 320;
    nasgul1Y         = 120;
    previousLevel = 0;

    SPRITE(0, spritePlayer, playerX, playerY, 1);
    SPRITE(1, spriteEnemyLeft,  nasgul1X,   nasgul1Y,   2);
    SpawnRing();

    gameClock = clock();

    /* ============================================================
       GLOWNA PETLA
       ============================================================ */
    while (1)
    {
        /* --------------------------------------------------
           GAME OVER — obslugiwany tutaj, nie przez rekurencje
           -------------------------------------------------- */
        if (needRestart)
        {
            

            needRestart = 0;

            /* ekran game over */
            *((unsigned char*)0xD015) = 0x00;
            InitHiRes();
            POKE(0xD020, 0);
            POKE(0xD021, 0);

            DrawText("GAME OVER",   14,  6, C64_WHITE, C64_BLACK);
            DrawText("SCORE",       10, 12, C64_WHITE, C64_BLACK);
            DrawText("DISTANCE",    10, 13, C64_WHITE, C64_BLACK);
            DrawText("TOTAL SCORE", 10, 14, C64_WHITE, C64_BLACK);
            DrawNumber(points,   24, 12, C64_WHITE, C64_BLACK);
            DrawNumber(distance, 24, 13, C64_WHITE, C64_BLACK);
            total = points + distance;
            DrawNumber(total,    24, 14, C64_WHITE, C64_BLACK);
            DrawText("press fire to continue", 9, 20, C64_DARKGRAY, C64_BLACK);

            /* czekaj az gracz PUŚCI fire */
            while (1) { joy = ~GetJoy(0); if (!(joy & JOY_BTN1)) break; }
            /* maly delay */
            for (i = 0; i < 30000; i++) {}
            /* czekaj na swiadome wcisnięcie */
            while (1) { joy = ~GetJoy(0); if (joy & JOY_BTN1) break; }

            /* reset wszystkich zmiennych */
            nasgul1Dir = 0;
            points        = 0;
            player_steps  = 0;
            distance      = 0;
            ringCollected = 0;
            level_map     = 1;
            playerX       = 100;
            playerY       = 110;
            first_spawn   = 1;
            nasgul1X         = 320;   /* daleko od gracza */
            nasgul1Y         = 120;
            previousLevel = 0;     /* wymusi redraw mapy */
            animFrame     = 0;
            animTimer     = 0;
            ring_power  = 200;
            ringActive  = 0;      
            orc1X         = 30;
            orc1Y         = 80;
            orc2X         = 280;
            orc2Y         = 80;
            orc3X         = 40;
            orc3Y         = 200;
            orc4X         = 280;
            orc4Y         = 200;

            /* wyczysc rejestry kolizji */
            dummy = *((unsigned char*)0xD01E);
            dummy = *((unsigned char*)0xD01F);
            (void)dummy;

            /* reset zegara */
            gameClock = clock();

            continue;   /* wróć na poczatek petli - swiezy start */
        }

        /* --------------------------------------------------
           SFX
           -------------------------------------------------- */
        SFX_Update();

        /* --------------------------------------------------
           ZMIANA MAPY / REDRAW
           -------------------------------------------------- */
        if (level_map != previousLevel)
        {
            oldLevel      = previousLevel;
            previousLevel = level_map;

            *((unsigned char*)0xD015) = 0x00;
            POKE(0xD006, 0);
            POKE(0xD007, 0);
            InitHiRes();

            LoadAndDrawMap(oldLevel);
            DrawHUD();
            DrawNumber(points, 33, 2, C64_WHITE, C64_BLACK);

            SPRITE(0, spritePlayer, playerX, playerY, 1);
            SPRITE_MOVE(1, nasgul1X, nasgul1Y);
            if ((level_map==3) || (level_map==2) || (level_map==4)) 
            {
              SPRITE(4, sprite1Orc, orc1X, orc1Y, 13);  
              SPRITE(5, sprite1Orc, orc2X, orc2Y, 13);  
              SPRITE(6, sprite1Orc, orc3X, orc3Y, 13); 
              SPRITE(7, sprite1Orc, orc4X, orc4Y, 13); 
            }
        

            if (!ringCollected && ring_map == level_map) {
                SPRITE(3, ring1, ring_pos_x[ring_slot], ring_pos_y[ring_slot], C64_LIGHTBLUE);
               *((unsigned char*)0xD015) |= 0x0F;
            } else {
               *((unsigned char*)0xD015) |= 0x07;
            }
            
        }

        /* --------------------------------------------------
           TAKT CZASOWY
           -------------------------------------------------- */
        if (clock() > gameClock)
        {
            gameClock = clock();

            /* animacja pierscienia */
            animTimer++;
            if (animTimer >= ANIM_SPEED && !ringCollected && ring_map == level_map)
            {
                animTimer = 0;
                animFrame ^= 1;
                if (animFrame == 0)
                    SPRITE(3, ring1, ring_pos_x[ring_slot], ring_pos_y[ring_slot], C64_LIGHTBLUE);
                else
                    SPRITE(3, ring2, ring_pos_x[ring_slot], ring_pos_y[ring_slot], C64_BLUE);
            }

            /* logika gry */
            CheckSpriteCollisions();
            CheckMapChange();
            if (level_map == 1) CheckCottage();

            /////////////////////////////////////////////////////////////////////////////////////////////////
            /*                      ruch wroga — zatrzymaj gdy pierscien aktywny */
            /////////////////////////////////////////////////////////////////////////////////////////////////
            
if (!ringActive)  //POTWORY ATAKUJA
{
     prevDir = nasgul1Dir;

    if (nasgul1Dir == 0) {
        if (nasgul1X > 24)  nasgul1X -= 1;
        else                nasgul1Dir = 1;
    } else {
        if (nasgul1X < 320) nasgul1X += 1;
        else                nasgul1Dir = 0;
    }

    /* zmien grafike TYLKO gdy zmienil sie kierunek */
    if (nasgul1Dir != prevDir) {
        if (nasgul1Dir == 0)
            SPRITE(1, spriteEnemyLeft, nasgul1X, nasgul1Y, 2);
        else
            SPRITE(1, spriteEnemy,     nasgul1X, nasgul1Y, 2);
    }

    SPRITE_MOVE(1, nasgul1X, nasgul1Y);
/////////////////////////////////
// Czekaj na konkretną linię rastra (np. 255 = dół ekranu)
//while (PEEK(0xD012) != 255);

if ((level_map==3) || (level_map==2) || (level_map==4)) 
    {
        orcAnimTimer++;
        //chase player
        if(orcAnimTimer >= 2)
        {
          MoveOrcs();
        orcAnimTimer = 0;
        }   
    SPRITE_MOVE(4, orc1X, orc1Y);
    SPRITE_MOVE(5, orc2X, orc2Y);
    SPRITE_MOVE(6, orc3X, orc3Y);
    SPRITE_MOVE(7, orc4X, orc4Y);
            
    
   }

}

            /* sterowanie */
            joy = ~GetJoy(0);

            if (joy & JOY_UP) {
                newX = playerX;
                newY = (signed int)playerY - SPEED;
                if (playerY > 90 &&
                    CanWalk(newX + HB_L, (unsigned int)newY + HB_T) &&
                    CanWalk(newX + HB_R, (unsigned int)newY + HB_T))
                {
                    playerY = (unsigned char)newY;
                    player_steps++;
                    if (player_steps >= 18) { distance++; player_steps = 0; }
                }
            }

            if (joy & JOY_DOWN) {
                newX = playerX;
                newY = (signed int)playerY + SPEED;
                if (CanWalk(newX + HB_L, (unsigned int)newY + HB_B) &&
                    CanWalk(newX + HB_R, (unsigned int)newY + HB_B))
                {
                    playerY = (unsigned char)newY;
                    player_steps++;
                    if (player_steps >= 18) { distance++; player_steps = 0; }
                }
            }

            if (joy & JOY_LEFT) {
                newX = playerX - SPEED;
                newY = playerY;
                if (CanWalk(newX + HB_L, (unsigned int)newY + HB_T) &&
                    CanWalk(newX + HB_L, (unsigned int)newY + HB_B))
                {
                    playerX = newX;
                    player_steps++;
                    if (player_steps >= 18) { distance++; player_steps = 0; }
                }
            }

            if (joy & JOY_RIGHT) {
                newX = playerX + SPEED;
                newY = playerY;
                if (CanWalk(newX + HB_R, (unsigned int)newY + HB_T) &&
                    CanWalk(newX + HB_R, (unsigned int)newY + HB_B))
                {
                    playerX = newX;
                    player_steps++;
                    if (player_steps >= 18) { distance++; player_steps = 0; }
                }
            }
      
            /* --- licznik trzymania fire --- */
       /* --- pierscien (fire) --- */
if (joy & JOY_BTN1) {
    if (ring_power > 0) {
        ring_power--;
        ringActive = 1;
        //SPRITE_DISABLE(0);

        /* odswież HUD tylko gdy seg sie zmienil (co 25 taktow) */
        {
            newSeg = (unsigned char)((200 - ring_power) / 25);
            if (newSeg != hudSeg) {
                hudSeg = newSeg;
                DrawHUD();
            }
        }
    } else {
        needRestart = 1;
    }
} else {
    if (ringActive) {
        ringActive = 0;
       //SPRITE_ENABLE(0);
        DrawHUD();   /* przywroc normalny wyglad po zwolnieniu */
    }
}
      
        SPRITE_MOVE(0, playerX, playerY);
        }
    }

    return 0;
}
