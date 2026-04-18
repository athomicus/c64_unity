// sprite.h
#ifndef SPRITE_H
#define SPRITE_H
#include <c64.h>
#include <string.h>

void SPRITE(unsigned char n, unsigned char *data, unsigned int x, unsigned char y, unsigned char color);
void SPRITE_MOVE(unsigned char n, unsigned int x, unsigned char y);

#endif /* SPRITE_H */