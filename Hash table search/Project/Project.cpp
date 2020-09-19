#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
// #include <Windows.h>

#include "hash.h"

// using namespace std;

/*************************************************
Function:       get_hash
Description:    负责计算给定字符串的哈希值
Input:          char* str：   char指针
Return:         unsigned int: 字符串的哈希值
*************************************************/
unsigned int get_hash(char* str) {
	return 1;
}



/*************************************************
Function:       output_result
Description:    把字符串写进输出文件
Input:          char* str：  指向要写入的字符串的指针
				FILE* file： 文件指针
*************************************************/
void output_result(char* str, FILE* file) {
	int status = fputs(str, file);	//TODO:if error
	fputc(10, file);
	if (status == -1) {

		// MessageBox(NULL, TEXT("Hello, bccn!"), TEXT("HelloMsg"), 0);

		//TODO: 弹窗提示保存出错
	}
}

int main() {
	FILE* f_string = NULL;
	// FILE* f_dict = NULL;
	// FILE* f_result = NULL;

	//TODO: initialize_hash_table;

	//TODO: build_hash_table;

	//TODO: search and output;

	f_string = fopen("../Data/string.txt", "r");
	if (!f_string) {
		return 0;
	}

	/*
	f_result = fopen("../Data/result.txt", "w");
	if (!f_result) {
		return 0;
	}
	*/


	for (size_t i = 0; i < 100; i++)
	{
		char str[99];
		fscanf(f_string, "%s", str, 99);
		printf("%12s 的hash值是：%u\n", str, get_hash(str));

		// output_result(str, f_result);

	}

	fclose(f_string);
}

