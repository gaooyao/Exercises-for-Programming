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
    int write_num=1;
    //打开模式为读入时不允许写入
	if(file_handler->open_status!=2){
		return 0;
	}
	
    //写入一行结果
    //char y=99;
    
    //不能对对象直接操作
 
    //当result的数据不为0时，result和写入的地址的指针向后移
	while(*str != 0) {
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = *str;
		str++;
	}
	//置入空行符
    file_handler->point++;
    *(file_handler->buffer + file_handler->point) = 10;
    write_num++;

    //file_handler->point++;
    //*str ++;
    //*(file_handler->buffer + file_handler->point)=* str;

	return 0;
}
