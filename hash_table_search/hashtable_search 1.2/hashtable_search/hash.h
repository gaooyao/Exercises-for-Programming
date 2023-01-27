#ifndef	node_struct
#define node_struct
#include "hash.c"

// SDBMHash�㷨
unsigned int SDBMHash(char* str);

// RS Hash�㷨
unsigned int RSHash(char* str);

// JS Hash�㷨
unsigned int JSHash(char* str);

// P.J.Weinberger Hash�㷨
unsigned int PJWHash(char* str);

// ELF Hash�㷨
unsigned int ELFHash(char* str);

// BKDR Hash�㷨
unsigned int BKDRHash(char* str);

// DJB Hash�㷨
unsigned int DJBHash(char* str);

// AP Hash�㷨
unsigned int APHash(char* str);

#endif
