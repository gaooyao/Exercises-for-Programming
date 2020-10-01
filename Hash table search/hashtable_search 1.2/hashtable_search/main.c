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
#include "gy.h"
#include "yn.h"
#include "murmurhash.h"


/* 变量的初始化 */
clock_t start, end;		//时间变量，用于统计运行时间
HashNode hash_table[23767] = { "",NULL };	//主Hash表
char list[256][256][256] = { 0 };	//一个三维数组，通过对比s_file字符串的前三个字节达到粗过滤，默认全为0
FileHandler* string_file;	//string.txt file handler
FileHandler* dict_file;		//dict.txt file handler	
FileHandler* result_file;	//result.txt file handler
char* str;


int main() {
    char *new_m;
    int m,k;
    m=num_m(0.0000001,1270574);
    printf("m=%d\n",m);
    k=num_k(m,1270574);
    printf("k=%d\n",k);
    new_m=create_m(m);
    uint64_t* out[2];
	/* 打开dict.txt，根据dict.txt建立内存 */
    dict_file = open_file("dict.txt", "r");
    while (read_line(dict_file, &str) == 1){
        for(int seed=0;seed<24;seed++){
            MurmurHash3_x64_128(str, strlen(str),seed,out);
            manipulate_m(new_m,(unsigned int)out[1]%m,1);
           // printf("%u\n",(unsigned int)out[1]%m);
           // printf("%ul\n",out[1]);
            //printf("%ul%ul\n",out[0],out[1]);
        }
        
    }
	/* 根据dict.txt建立内存结束 */
	/* 打开string.txt，根据string.txt查询 */
	


	/* 根据string.txt查询结束 */
	/* 关闭文件，结束程序 */
	close_file(dict_file);
    return 0;
    
	/* 打开用到的文件 */
	start = clock();
	string_file = open_file("string.txt", "r");
	dict_file = open_file("dict.txt", "r");
	result_file = open_file("result.txt", "w");
	end = clock();
	printf("File Opened successfully. It takes %f seconds.\n", (float)(end - start) / CLOCKS_PER_SEC);

	/* 根据string.txt构建hash表 */
	printf("Building the hash table now...\n");
	int ct_num = 0;		//变量ct记录发生冲突次数
    while (read_line(string_file, &str) == 1) {
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
	start = clock();//
	printf("Construction of Hash Completed, it takes %f seconds, conflicts occurred %d times\n", (float)(start - end) / CLOCKS_PER_SEC, ct_num);

	/* 开始搜索，依次取dict.txt的每一行，在构建好的hash表中搜索 */
	printf("Start Searching...\n");
	int total_line = 0;		//dict.txt文本的总行数
	int hash_line = 0;		//需要计算hash值的行数
	int success_line = 0;	//成功在hash表中找到的行数
	while (read_line(dict_file, &str)) {
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
					write_line(result_file, str);
					break;//终止所在层(while)的循环
				}
				node_pointer = node_pointer->next;	//若读取到的字符串与指针指向节点的字符串不相同则指针后移，直到最后还未成功匹配则查找失败
                
			}
		    }
	}
	end = clock();
	printf("Searching Completed. It takes %f seconds. \n", (float)(end - start) / CLOCKS_PER_SEC);//搜索完成，共用时%f秒

	/* 搜索任务完成，显示搜索结果并退出 */
	printf("Task Completed. It is included %d lines. %d lines remaining after first filtering, %d lines have been found.\n", total_line, hash_line, success_line);//任务完成，一共 %d 行，首次过滤后剩余 %d 行，搜索到 %d 行。
	close_file(string_file);
	close_file(dict_file);
	close_file(result_file);
	system("pause");
	return 0;
    
}
