#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "struct.h"
#include "hash.h"


int get_hash(char* str) {
    unsigned int BKDRHash(char* str);
	return BKDRHash(str) % 23767;
}


int write_line(FileHandler* file_handler, char* str) {
	while (*str != 0) {
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = *str;
		str++;
	}
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 10;
	return 0;
}
