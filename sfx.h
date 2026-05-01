// sfx.h — tylko funkcje, bez extern zmiennych
#ifndef SFX_H
#define SFX_H

#define SFX_NONE    0
#define SFX_RING    1
#define SFX_HURT    2
#define SFX_LEVELUP 4

void SFX_Play(unsigned char id);
void SFX_Update(void);

#endif