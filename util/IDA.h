#pragma warning(push)
#pragma warning(disable:4005)

#define LOBYTE(x) (*((BYTE *)&(x)))
#define LOWORD(x) (*((WORD *)&(x)))
#define LODWORD(x) (*((DWORD *)&(x)))
#define HIBYTE(x) (*((BYTE *)&(x) + 1))
#define HIWORD(x) (*((WORD *)&(x) + 1))
#define HIDWORD(x) (*((DWORD *)&(x) + 1))

#define BYTEn(x, n) (*((BYTE *)&(x) + n))
#define BYTE1(x) BYTEn(x, 1)
#define BYTE2(x) BYTEn(x, 2)
#define BYTE3(x) BYTEn(x, 3)
#define BYTE4(x) BYTEn(x, 4)

#define SBYTEn(x, n) (*((__int8 *)&(x) + n))
#define SWORDn(x, n) (*((__int16 *)&(x) + n))
#define SBYTE1(x) SBYTEn(x, 1)
#define SBYTE2(x) SBYTEn(x, 2)
#define SBYTE3(x) SBYTEn(x, 3)
#define SBYTE4(x) SBYTEn(x, 4)

#pragma warning(pop)