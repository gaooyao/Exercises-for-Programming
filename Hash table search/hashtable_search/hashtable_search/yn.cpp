#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "hash.h"

int get_hash(char* str) {
	return BKDRHash(str) % 23767;
}

void output_result(FILE* file, char* str) {
	//printf("%s", str);
}