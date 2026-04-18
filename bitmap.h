// bitmap.h
#ifndef BITMAP_H
#define BITMAP_H

#include <c64.h>
#include <string.h>

void BITMAP(const unsigned char* data);
void BITMAP_OFF(void);
void BITMAP_COLOR(unsigned char foreground, unsigned char background);

#endif /* BITMAP_H */
