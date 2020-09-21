#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hash.h"

//typedef struct SmallStr {
//	char str[10];
//};
//typedef struct BigStr {
//	char str[100];
//};

typedef struct HashNode {
	char str[100];
	HashNode* next;
};


int get_hash(char* str) {
	return BKDRHash(str) % 23767;
}

//void create_node_str(HashNode* node, char* str) {
//	if (strlen(str) < 10) {
//		SmallStr str_node;
//		strcpy(str_node.str, str);
//		node->small_str = &str_node;
//		printf("MMMMMMM%s\n", node->small_str->str);
//	}
//	else {
//		BigStr str_node;
//		strcpy(str_node.str, str);
//		node->big_str = &str_node;
//	}
//}


int main() {

	clock_t start, end;
	HashNode hash_table[23767] = { "",NULL };
	bool list[256][256][256] = { NULL };
	FILE* string_file = NULL;
	FILE* dict_file = NULL;
	char str[99] = { 0 };
	start = clock();
	printf("开始打开所需文件");
	string_file = fopen("string.txt", "r");
	if (!string_file) {
		printf("打开string.txt文件失败！");
		return 0;
	}
	dict_file = fopen("dict.txt", "r");
	if (!dict_file) {
		printf("打开dict.txt文件失败！");
		return 0;
	}
	end = clock();
	printf("文件打开完成，共用时%f秒。\n", (double)(end - start) / CLOCKS_PER_SEC);

	printf("开始根据string.txt构建hash表");
	int ct = 0;
	while (fscanf(string_file, "%s", str) != -1) {
		unsigned short int first_char = str[0] & 0xff;
		unsigned short int second_char = str[1] & 0xff;
		unsigned short int third_char = str[2] & 0xff;
		list[first_char][second_char][third_char] = true;
		int hash_key = get_hash(str);
		if (!strlen(hash_table[hash_key].str)) {
			strcpy(hash_table[hash_key].str, str);
		}
		else {
			HashNode* new_node = new HashNode{ "",NULL };
			strcpy(new_node->str, str);
			HashNode* node_pointer = &hash_table[hash_key];
			while (node_pointer->next) {
				ct++;
				node_pointer = node_pointer->next;
			}
			node_pointer->next = new_node;
		}
	}
	start = clock();
	printf("构建hash表完成，用时%f秒，发生冲突%d次\n", (double)(start - end) / CLOCKS_PER_SEC, ct);
	printf("开始搜索");
	int t = 0;
	int k = 0;
	int l = 0;
	while (fscanf(dict_file, "%s", str) != -1) {
		t++;
		unsigned short int first_char = str[0] & 0xff;
		unsigned short int second_char = str[1] & 0xff;
		unsigned short int third_char = str[2] & 0xff;
		if (list[first_char][second_char][third_char]) {
			k++;
			int hash_key = get_hash(str);
			HashNode* node_pointer = &(hash_table[hash_key]);
			while (node_pointer) {
				if (!strcmp(node_pointer->str, str)) {
					l++;
					break;
				}
				node_pointer = node_pointer->next;
			}
		}
	}
	end = clock();
	printf("搜索完成，共用时%f秒，一共%d行，首次过滤后剩余%d行，搜索到%d行。\n", (double)(end - start) / CLOCKS_PER_SEC, t, k, l);
	return 0;

}