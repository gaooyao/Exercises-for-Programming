#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "gy.h"
#include "hash.h"



int get_hash(char* str) {
    unsigned int BKDRHash(char* str);
	return BKDRHash(str) % 23767;
}

void output_result(FileHandler* file, char* str) {
	printf("%s", str);
    
    //fclose(file);
    
}
