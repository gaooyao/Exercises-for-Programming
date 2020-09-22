/**********************************************************************
*
* 文件名称：main.cpp
* 内容摘要：计算机应用编程实验-实验1主程序
* 作    者：Jahn1998，gaooyao
* 仓库地址：https://github.com/Jahn1998/Exercises-for-Programming
* 项目日期：2020.9
*
**********************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "struct.h"

#include "gy.cpp"
#include "yn.cpp"


/* 变量的初始化 */
clock_t start, end;		//时间变量，用于统计运行时间
HashNode hash_table[23767] = { "",NULL };	//主Hash表
char list[256][256][256] = { 0 };	//一个三维数组，通过对比字符串的前三个字节达到粗过滤，默认全为0
FILE* string_file = NULL;	//string.txt file handler
FILE* dict_file = NULL;		//dict.txt file handler
FILE* result_file = NULL;	//result.txt file handler
char str[99] = { 0 };	//临时字符串，用于存放从文件扫描的字符串


int main() {

	/* 打开用到的文件 */
	start = clock();
	printf("open the needle file...");
	string_file = open_file("string.txt", "r");
	dict_file = open_file("dict.txt", "r");
	result_file = open_file("result.txt", "w");
	end = clock();
	printf("open file success,spent %f second.\n", (float)(end - start) / CLOCKS_PER_SEC);


	/* 根据string.txt构建hash表 */
	printf("build the hash table now...");
	int ct_num = 0;		//变量ct记录发生冲突次数
	while (fscanf(string_file, "%s", str) != -1) {
		list[str[0] & 0xff][str[1] & 0xff][str[2] & 0xff] = (char)1;	//此字符串的前三个字节出现，在三维数组中对应位置标记为1
		int hash_key = get_hash(str);
		if (!strlen(hash_table[hash_key].str)) {	//若hash表中对应位置的node为空，则把其文本内容设置为当前字符串
			strcpy(hash_table[hash_key].str, str);
		}
		else {	//若hash表中对应位置的node不为空，则创建新node放在node链的末尾
			HashNode* new_node = create_new_node(str);			//创建新的node
			HashNode* node_pointer = &hash_table[hash_key];		//node指针先指向node链第一个节点
			while (node_pointer->next) {	//循环遍历，直到找到最后一个节点
				ct_num++;	//若node_pointer指向的节点不是最后一个节点，冲突次数加一
				node_pointer = node_pointer->next;
			}
			node_pointer->next = new_node;		//最后一个node的next指针指向新创建的node
		}
	}
	start = clock();
	printf("构建hash表完成，用时%f秒，发生冲突%d次\n", (float)(start - end) / CLOCKS_PER_SEC, ct_num);


	/* 开始搜索，依次取dist.txt的每一行，在构建好的hash表中搜索 */
	printf("开始搜索...\n");
	int total_line = 0;		//总行数
	int hash_line = 0;		//需要计算hash值的行数
	int success_line = 0;	//成功在hash表中找到的行数
	while (fscanf(dict_file, "%s", str) != -1) {
		total_line++;
		if (list[str[0] & 0xff][str[1] & 0xff][str[2] & 0xff]) {	//粗过滤，若被找字符串的前三个字节未在字典中出现过，则直接跳过
			//前三个字节出现过，需要进hash表精确查找
			hash_line++;
			int hash_key = get_hash(str);
			HashNode* node_pointer = &(hash_table[hash_key]);	//创建指针指向hash表中对应位置的首节点
			while (node_pointer) {
				if (!strcmp(node_pointer->str, str)) {	//文件读取到的字符串与指针指向节点的字符串对比，若相同则说明此字符串出现过，搜索成功
					//查找成功，计数并保存到输出文件
					success_line++;
					output_result(result_file, str);
					break;
				}
				node_pointer = node_pointer->next;	//若读取到的字符串与指针指向节点的字符串不相同则指针后移，直到最后还未成功匹配则查找失败
			}
		}
	}
	end = clock();
	printf("搜索完成，共用时%f秒。\n", (float)(end - start) / CLOCKS_PER_SEC);


	/* 搜索任务完成，显示搜索结果并退出 */
	printf("任务完成，一共 %d 行，首次过滤后剩余 %d 行，搜索到 %d 行。\n", total_line, hash_line, success_line);
	fclose(string_file);
	fclose(dict_file);
	fclose(result_file);
	system("pause");
	return 0;
}
