#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

typedef struct HashNode {
	char a;
	char b;
	HashNode* next;
};



int main() {
	HashNode a;
	HashNode* list[256][256][256] = { NULL };
	FILE* string_file = NULL;
	FILE* dict_file = NULL;
	char str[99] = { 0 };
	string_file = fopen("string.txt", "r");
	dict_file = fopen("dict.txt", "r");

	if (!string_file) {
		printf("打开string.txt文件失败！");
		return 0;
	}
	while (fscanf(string_file, "%s", str) != -1) {
		unsigned short int first_char = str[0] & 0xff;
		unsigned short int second_char = str[1] & 0xff;
		unsigned short int third_char = str[2] & 0xff;
		//printf("%s %d %d %d\n", str, first_char, second_char, third_char);
		list[first_char][second_char][third_char] = &a;
		printf("%d\n", JSHash(str));
	}
	int t = 0;
	int k = 0;
	while (fscanf(dict_file, "%s", str) != -1) {
		t++;
		unsigned short int first_char = str[0] & 0xff;
		unsigned short int second_char = str[1] & 0xff;
		unsigned short int third_char = str[2] & 0xff;
		// printf("%s %d %d\n", str, first_char, second_char);
		if (list[first_char][second_char][third_char]) {
			k++;
		}
	}
	printf("%d %d\n",t,k);

	return 0;

}