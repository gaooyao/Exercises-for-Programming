#ifndef	node_struct
#define node_struct

#include "hash.c"

// SDBMHash算法
unsigned int SDBMHash(char* str);

// RS Hash算法
unsigned int RSHash(char* str);

// JS Hash算法
unsigned int JSHash(char* str);

// P.J.Weinberger Hash算法
unsigned int PJWHash(char* str);

// ELF Hash算法
unsigned int ELFHash(char* str);

// BKDR Hash算法
unsigned int BKDRHash(char* str);

// DJB Hash算法
unsigned int DJBHash(char* str);

// AP Hash算法
unsigned int APHash(char* str);

#endif
