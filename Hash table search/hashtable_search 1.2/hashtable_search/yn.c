#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "struct.h"
#include "hash.h"
#include <math.h>
#include "yn.h"
#include "murmurhash.h"
#include <stdint.h>


/*
 * the Name of this : get_hash
 * the Function of this : Calculate the Hash Key
 * Parameters : str : every str has its own Hash Key
 * value of return : int, value of the Hash key
*/

int get_hash(char* str) {
	unsigned int BKDRHash(char* str);
	return BKDRHash(str) % 23767;
}

/*
* the Name of this  : write_line
* the Function of this : write the targeted lines into result.txt
* Parameter 1 : FileHandler* file_handler : where to be exported
* Parameter 2 : char* str : what to be exported
* value of return : int, 0
*/

int write_line(FileHandler* file_handler, char* str) {


	//if the file is not under the writing status
	if (file_handler->open_status != 2) {
		return 0;
	}
	file_handler->point++;
    
    //di
	*(file_handler->buffer + file_handler->point) = 0xB5;
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 0xDA;
	
    //convert the numbers to be auto-changed
	char target[20];//we have 6306 numbers
	static int num = 1;//numbers will change
	char* t = target;
	sprintf(target,"%d",num);

	while (*t != 0) {
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = *t;
        t++ ;
	}
    num++;

    //hang:
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

/*
* the Name of this  : num_m
* the Function of this : calculate the number of memory that we need
* Parameter 1 : float f : the error that we allow=0.0000001
* Parameter 2 : int n : the number of the char=1270574
* value of return : int
*/
int num_m(float f, int n)
{
    float m;
    m=n*1.44*log2(1/f);
    m=m/1+1;
    return (int)m;//m wei
};
/*
* the Name of this  : num_k
* the Function of this : calculate the number of Hash that we need
* Parameter 1 : int m : the number of memory that we need
* Parameter 2 : int n : the number of the char=1270574
* value of return : int
*/
int num_k(int m, int n)
{
    float k;
    k=0.693*m/n;
    k=k/1+1;
    return (int)k;
};
/*
 */

char* create_m(int m){
    m=m/8+1;
    char* new_m = (char*)malloc(m);//malloc zijie
    for(int i=0;i<m;i++)
    {
        *(new_m+i)=0;
    };
    return new_m;
}



