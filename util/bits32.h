#pragma once

#define BitsGet(x,y) (x & (1 << (y & 31)))
#define BitsSet(x,y) x |= (1 << (y & 31))
#define BitsUnSet(x,y) x &= ~(1 << (y & 31))
#define BitsSwitch(x,y) x ^= (1 << (y & 31))

extern int BitsCount(unsigned x);
extern int BitsGetFirst(unsigned x);
extern int BitsGetRandom(unsigned x);