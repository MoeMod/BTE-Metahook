#include <bits32.h>
#include "time.h"
#include "stdlib.h"

int BitsCount(unsigned x)
{
	x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
	x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
	x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);
	return x;
}
/*int BitsGetFirst(unsigned x)
{
	int iPos = 0;
	for (int i = 4; i >= 0; i--)
		if ((x & ((1 << (1 << i)) - 1)) == 0)
		{
			x >>= (1 << i);
			iPos += (1 << i);
		}
	return iPos;
}*/
int BitsGetRandom(unsigned x)
{
	int iCount = BitsCount(x);
	while (x)
	{
		iCount--;
		srand(time(NULL));
		if (!(rand() % (iCount + 1)) || !iCount)
			return BitsGetFirst(x);
		else
			x &= x - 1;
	}
	return -1;
}