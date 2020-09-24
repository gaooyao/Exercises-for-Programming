#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "struct.h"
#include "hash.h"

#include "yn.h"


int get_hash(char* str) {
	unsigned int BKDRHash(char* str);
	return BKDRHash(str) % 23767;
}



int write_line(FileHandler* file_handler, char* str) {

	//若文件打开类型不是写入类型，则不允许写入
	if (file_handler->open_status != 2) {
		return 0;
	}
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 0xB5;
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 0xDA;
	

	char bb[20];
	static int b = 1;
	char* d = bb;
	sprintf(bb,"%d",b);

	while (*d != 0) {
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = *d;
		d++ ;
	}
	b++;

	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 0xD0;
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 0xD0;
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 0x3A;
	//把字符串写入缓冲区中
	while (*str != 0) {
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = *str;
		str++;
	}
	//查入换行符
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 10;
	return 0;
}
