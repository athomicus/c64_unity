#include <c64.h>
#include "unity.h"
#include <time.h>
#include "sprite.h"
#include "sprites_data.h"
#include "bitmap.h"
#include "font_hud.h"

#define HB_L  6
#define HB_R 10
#define HB_T  6
#define HB_B 10
#define COTTAGE_X1  280
#define COTTAGE_Y1  210
#define COTTAGE_X2  312
#define COTTAGE_Y2  242
#define SPEED       3
#define ANIM_SPEED  9
#define SPRITE_ENABLE(slot)   *((unsigned char*)0xD015) |=  (1 << (slot))
#define SPRITE_DISABLE(slot)  *((unsigned char*)0xD015) &= ~(1 << (slot))

unsigned int  points        = 0;
unsigned char ringCollected = 0;
unsigned int  playerX       = 100;
unsigned char playerY       = 110;
unsigned char level_map     = 1;

static const unsigned int  ring_pos_x[8] = { 80, 140, 200, 260,  60, 120, 180, 240 };
static const unsigned char ring_pos_y[8] = { 100, 140, 180, 120, 110, 150, 130, 160 };

unsigned char ring_slot   = 0;
unsigned char ring_map    = 1;
unsigned char first_spawn = 1;

// ============================================================
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

// ============================================================
void DrawHUD(void)
{
    *((unsigned char*)0xD015) = 0x00;
    DRAW_TILE(n01, 0,  0, 4, 4, 11, 0);
    DRAW_TILE(n02, 4,  0, 4, 4, 11, 0);
    DRAW_TILE(n03, 8,  0, 4, 4, 11, 0);
    DRAW_TILE(n04, 12, 0, 4, 4, 11, 0);
    DRAW_TILE(n05, 16, 0, 4, 4, 11, 0);
    DRAW_TILE(n02, 20, 0, 4, 4, 11, 0);
    DRAW_TILE(n03, 24, 0, 4, 4, 11, 0);
    DRAW_TILE(n05, 28, 0, 4, 4, 11, 0);
    *((unsigned char*)0xD015) = 0x0F;
}

// ============================================================
void InitHiRes(void)
{
    *((unsigned char*)0xDD00) = (*((unsigned char*)0xDD00) & 0xFC) | 0x02;
    memset((unsigned char*)0x6000, 0x00, 8000);
    memset((unsigned char*)0x4800, 0x00, 1000);
    VIC.ctrl1 |=  0x20;
    VIC.ctrl2 &= ~0x10;
    VIC.addr   =  0x28;
}

// ============================================================
void DrawMap(const unsigned char map[][20])
{
    unsigned char x, y;
    for (y = 0; y < 10; y++) {
        for (x = 0; x < 20; x++) {
            switch (map[y][x]) {
                case 1:  DRAW_TILE(tree,         x*2, y*2+5, 2, 2, C64_BLACK,     C64_GREEN);  break;
                case 2:  DRAW_TILE(tree2,        x*2, y*2+5, 2, 2, C64_BLACK,     C64_GREEN);  break;
                case 4:  DRAW_TILE(roslina,      x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case 5:  DRAW_TILE(stone,        x*2, y*2+5, 2, 2, C64_LIGHTGRAY, C64_BLACK);  break;
                case 6:  DRAW_TILE(tree_left,    x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case 7:  DRAW_TILE(tree_center,  x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case 8:  DRAW_TILE(tree_right,   x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
                case 9:  DRAW_TILE(tree_up,      x*2, y*2+5, 2, 2, C64_GREEN,     C64_BLACK);  break;
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

// ============================================================
// SpawnRing — losuje mape i pozycje
// pierwszy spawn zawsze na mapie 1
// ============================================================
void SpawnRing(void)
{
    if (first_spawn) {
        ring_map    = 1;
        first_spawn = 0;
    } else {
        ring_map = (GetRandom() & 0x01) + 1;
    }
    ring_slot = GetRandom() & 0x07;

    if (ring_map == level_map) {
        SPRITE(3, ring1, ring_pos_x[ring_slot], ring_pos_y[ring_slot], C64_LIGHTBLUE);
        *((unsigned char*)0xD015) |= 0x08;
    } else {
        SPRITE_DISABLE(3);
    }
}

// ============================================================
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
    }
}

// ============================================================
void CheckMapChange(void)
{
    if (level_map == 1) {
        if ((playerX > 82) && (playerX < 106) && (playerY > 230)) level_map = 2;
    } else if (level_map == 2) {
        if ((playerX > 30) && (playerX < 106) && (playerY < 100)) level_map = 1;
    }
}

// ============================================================
void CheckSpriteCollisions(void)
{
    unsigned char col = *((unsigned char*)0xD01E);
    if ((col & 0x01) && (col & 0x08)) {
        if (!ringCollected) {
            points += 10;
            DrawNumber(points, 33, 2, C64_WHITE, C64_BLACK);
            SPRITE_DISABLE(3);
            ringCollected = 1;
        }
    }
}

// ============================================================
unsigned char CanWalk(unsigned int px, unsigned int py)
{
    unsigned char tx, ty;
    tx = (unsigned char)((px - 24) / 16);
    ty = (unsigned char)((py - 90) / 16);
    if (level_map == 1) return (mapa1[ty][tx] == 0) ? 1 : 0;
    if (level_map == 2) return (mapa2[ty][tx] == 0) ? 1 : 0;
    return 0;
}

// ============================================================
//                         MAIN
// ============================================================
int main(void)
{
    unsigned int  shipX         = 280;
    unsigned char shipY         = 120;
    unsigned int  newX, newY;
    unsigned char animFrame     = 0;
    unsigned char animTimer     = 0;
    unsigned char previousLevel = 0;
    unsigned int  joy;
    clock_t gameClock = clock();

    InitSFX();
    InitRandom();
    InitHiRes();
    POKE(0xD020, 0);
    POKE(0xD021, 0);

    // --------------------------------------------------------
    //  INTRO
    // --------------------------------------------------------
    DRAW_TILE(n01, 0,  0, 4, 4, 2, 0);
    DRAW_TILE(n02, 4,  0, 4, 4, 2, 0);
    DRAW_TILE(n03, 8,  0, 4, 4, 2, 0);
    DRAW_TILE(n04, 12, 0, 4, 4, 2, 0);
    DRAW_TILE(n05, 16, 0, 4, 4, 2, 0);
    DRAW_TILE(n02, 20, 0, 4, 4, 2, 0);
    DRAW_TILE(n03, 24, 0, 4, 4, 2, 0);
    DRAW_TILE(n05, 28, 0, 4, 4, 2, 0);
    DrawText("One Ring to rule them all",              6,  7, C64_WHITE,    C64_BLACK);
    DrawText("One Ring to find them",                  9,  9, C64_WHITE,    C64_BLACK);
    DrawText("One Ring to bring them all",             6, 11, C64_WHITE,    C64_BLACK);
    DrawText("and in the darkness bind them",          5, 13, C64_WHITE,    C64_BLACK);
    DrawText("Find the other Rings avoid enemies",     3, 18, C64_DARKGRAY, C64_BLACK);
    DrawText("bring the Rings to the hobbits hut",     2, 19, C64_DARKGRAY, C64_BLACK);
    DrawText("Use the ONE RING",                      10, 21, C64_DARKGRAY, C64_BLACK);
    DrawText("press fire button to become invisible",   0, 22, C64_DARKGRAY, C64_BLACK);
    DrawText("but it slowly kills you",                7, 23, C64_DARKGRAY, C64_BLACK);
    DrawText("Getting caught means instant death",     2, 24, C64_DARKGRAY, C64_BLACK);

    while (1) {
        joy = ~GetJoy(0);
        if (joy & JOY_BTN1) break;
    }

    // --------------------------------------------------------
    //  START GRY
    // --------------------------------------------------------
    level_map = 1;
    playerX   = 100;
    playerY   = 110;

    SPRITE(0, spritePlayer, playerX, playerY, 1);
    SPRITE(1, spriteEnemy,  120, 80, 2);
    SPRITE(2, spriteOrc,    50,  50, 5);

    SpawnRing();  // pierwsze losowanie — zawsze mapa 1

    // --------------------------------------------------------
    //  MAIN LOOP
    // --------------------------------------------------------
    while (1) {

        // Zmiana levelu (previousLevel==0 wymusza rysowanie przy starcie)
        if (level_map != previousLevel)
        {
            previousLevel = level_map;

            *((unsigned char*)0xD015) = 0x00;
            InitHiRes();

            if (level_map == 1) {
                playerX = 90; playerY = 200;
                DrawMap(mapa1);
                DrawHUD();
                DRAW_TILE(hobbit_small, 34, 20, 4, 4, 9, 0);
            }
            if (level_map == 2) {
                playerX = 90; playerY = 120;
                DrawMap(mapa2);
                DrawHUD();
            }

            SPRITE(0, spritePlayer, playerX, playerY, 1);
            SPRITE_MOVE(1, shipX, shipY);

            // Pokaz pierscion tylko jesli jest na tej mapie i nie zebrany
            if (!ringCollected && ring_map == level_map) {
                SPRITE(3, ring1, ring_pos_x[ring_slot], ring_pos_y[ring_slot], C64_LIGHTBLUE);
                *((unsigned char*)0xD015) = 0x0F;  // sprite'y 0,1,2,3
            } else {
                *((unsigned char*)0xD015) = 0x07;  // sprite'y 0,1,2 — bez ring
            }

            DrawNumber(points, 33, 2, C64_WHITE, C64_BLACK);
        }

        // Takt czasowy
        if (clock() > gameClock)
        {
            gameClock = clock();

            // Animacja pierscienia — tylko gdy na tej mapie i nie zebrany
            animTimer++;
            if (animTimer >= ANIM_SPEED && !ringCollected && ring_map == level_map) {
                animTimer = 0;
                animFrame ^= 1;
                if (animFrame == 0)
                    SPRITE(3, ring1, ring_pos_x[ring_slot], ring_pos_y[ring_slot], C64_LIGHTBLUE);
                else
                    SPRITE(3, ring2, ring_pos_x[ring_slot], ring_pos_y[ring_slot], C64_BLUE);
            }

            CheckSpriteCollisions();
            CheckMapChange();
            if (level_map == 1) CheckCottage();

            if (shipX > 24) shipX -= 1;
            else            shipX = 320;
            SPRITE_MOVE(1, shipX, shipY);

            joy = ~GetJoy(0);

            if (joy & JOY_UP) {
                newX = playerX;
                newY = playerY - SPEED;
                if (CanWalk(newX+HB_L, newY+HB_T) && CanWalk(newX+HB_R, newY+HB_T))
                    playerY = (unsigned char)newY;
            }
            if (joy & JOY_DOWN) {
                newX = playerX;
                newY = playerY + SPEED;
                if (CanWalk(newX+HB_L, newY+HB_B) && CanWalk(newX+HB_R, newY+HB_B))
                    playerY = (unsigned char)newY;
            }
            if (joy & JOY_LEFT) {
                newX = playerX - SPEED;
                newY = playerY;
                if (CanWalk(newX+HB_L, newY+HB_T) && CanWalk(newX+HB_L, newY+HB_B))
                    playerX = newX;
            }
            if (joy & JOY_RIGHT) {
                newX = playerX + SPEED;
                newY = playerY;
                if (CanWalk(newX+HB_R, newY+HB_T) && CanWalk(newX+HB_R, newY+HB_B))
                    playerX = newX;
            }

            SPRITE_MOVE(0, playerX, playerY);
        }
    }

    return 0;
}