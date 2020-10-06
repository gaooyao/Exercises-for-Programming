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
#ifndef	node_struct
#define node_struct

typedef struct HashNode {
	char str[100];
	struct HashNode* next;
}HashNode;

/* �ļ�����Handler,�����ļ��������ӿ��д�ٶ� */
typedef struct FileHandler {
	FILE* file;		//���������ļ�ָ��
	char file_name[99];		//�ļ���
	int open_status;		//��״̬��Ϊ0ʱΪ�رգ�1Ϊreadģʽ��2writeģʽd
	int point;//ָ���ڵ�ǰ�������еĶ�дλ��
	unsigned int buffer_size;//�ļ��������Ĵ�С
	char* buffer;//�ļ�������
}FileHandler;

#endif

//
//  gy.h
//  hashtable_search
//
//  Created by 杨恩 on 2020/9/22.
//



HashNode* create_new_node(char* str);//创建新的hash节点
FileHandler* open_file(char* file_name, char* open_type);//通过文件接口打开新文件
int read_line(FileHandler* file_handler, char** str);//从文件中读取一行
int close_file(FileHandler* file_handler);//关闭文件


/**********************************************************************
*
* 文件名称：gy.cpp
* 内容摘要：计算机应用编程实验-实验1-gaooyao的任务函数
* 作    者：gaooyao
* 仓库地址：https://github.com/Jahn1998/Exercises-for-Programming
* 项目日期：2020.9
*
**********************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int FILE_BUFFER_SIZE = 1048576;    //文件缓冲区大小，单位字节

/*
* 函数名称：create_new_node
* 函数功能：负责根据给定内容创建新的hash节点
* 参数：	str：新创建节点的文本内容
* 返回值：HashNode指针
*/
HashNode* create_new_node(char* str) {
	HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));	//创建空节点
	if (new_node) {
		//创建成功则给其赋值
		new_node->str[0] = '\0';//default value
		new_node->next = NULL;
		strcpy(new_node->str, str);
		return new_node;
	}
	else {
		//创建失败则显示错误信息并退出程序
		printf("create node fail!");
		system("stop");
		return new_node;
	}
	return new_node;
}

/*
* 函数名称：open_file
* 函数功能：根据给定属性创建新的FileHandler对象
* 参数：	file_name：要打开的文件名
*		open_type:文件打开类型，如"r","w"
* 返回值：FileHandler指针
*/
FileHandler* open_file(char* file_name, char* open_type) {
	/* 创建FileHandler */
	FileHandler* file_handler = (FileHandler*)malloc(sizeof(FileHandler));
	if (!file_handler) {
		printf("open %s fail!", file_name);
		return NULL;
	}
	/* 打开文件 */
	FILE* _file = (FILE*)malloc(sizeof(FILE));
	_file = fopen(file_name, open_type);
	if (!_file) {
		printf("open %s fail!", file_name);
		return NULL;
	}
	file_handler->file = _file;

	/* 创建文件缓冲buffer */
	file_handler->buffer_size = FILE_BUFFER_SIZE;
	file_handler->buffer = (char*)malloc(file_handler->buffer_size);
	if (!file_handler->buffer) {
		fclose(file_handler->file);
		return 0;	//分配内存失败
	}

	/* FileHandler属性初始化 */
	strcpy(file_handler->file_name, file_name);
	file_handler->open_status = (open_type == "r" ? 1 : 2);//三元表达式
	file_handler->point = -1;
	return file_handler;
}

/*
* 函数名称：read_line
* 函数功能：从文件中读取一行文本
* 参数：	file_handler：被读取的FileHandler对象
*		str：存放被读取文本的地址
* 返回值：int，1为读取成功，0为读取失败
* TODO:已知bug：若文件结尾为一空行则不会被输出
*/
int read_line(FileHandler* file_handler, char** str) {
	if (file_handler->open_status != 1) return 0;	//打开模式为写入时不允许读取
	int old_point = file_handler->point;//记录当前指针位置
	do {
		//如果已读到文件末尾则返回
		if (file_handler->point != -1 && *(file_handler->buffer + file_handler->point) == -1)return 0;

		//根据当前指针判断是否需要读入新数据
		int is_read_new_data = 0;
		//如果buffer为空，则读入新buffer
		if (file_handler->point == -1) {
			int read_num;
			read_num = fread(file_handler->buffer, 1, file_handler->buffer_size, file_handler->file);
			if (!read_num) {
				return 0;
			}
			else {
				is_read_new_data = 1;
			}
		}
		//如果buffer内不足一行，则读入新数据
		if (file_handler->point + 1 >= file_handler->buffer_size) {
			//copy buffer尾部数据至头部
			int copy_num = 0;
			while (old_point < file_handler->buffer_size) {
				*(file_handler->buffer + copy_num) = *(file_handler->buffer + old_point + 1);
				copy_num++;
				old_point++;
			}
			//读入新数据
			int read_num;
			read_num = fread((char*)(file_handler->buffer + copy_num), 1, file_handler->buffer_size - copy_num, file_handler->file);
			if (!read_num) {
				return 0;
			}
			else {
				is_read_new_data = 1;
			}
		}
		//若读入新数据则重置buffer指针
		if (is_read_new_data) {
			file_handler->point = 0;
			//判断读入的新数据是否可用
			while (*(file_handler->buffer + file_handler->point) != 10 && *(file_handler->buffer + file_handler->point) != 0 && *(file_handler->buffer + file_handler->point) != -1) {
				file_handler->point++;
				if (file_handler->point > file_handler->buffer_size) {
					//读取到的整个buffer内容少于一行，则超过最大处理大小//-1结束//10换行符
					system("stop");
					return 0;
				}
			}
			//数据读取成功，str指针指向字符串开头并返回
			*(file_handler->buffer + file_handler->point) = 0;//断行
			(*str) = &(file_handler->buffer[0]);
			return 1;
		}
		file_handler->point++;
	} while (*(file_handler->buffer + file_handler->point) != '\n');//point指针循环后移，直到遇到换行符则停止
	//没有读入新数据，但已找到被读文本的开头和结尾
	*(file_handler->buffer + file_handler->point) = 0;//断行
	(*str) = (file_handler->buffer + old_point + 1);
	return 1;
}

/*r
* 函数名称：close_file
* 函数功能：关闭文件，释放FileHandler对象
* 参数：	file_handler：被关闭的FileHandler对象
* 返回值：int,是否关闭成功
*/
int close_file(FileHandler* file_handler) {
	//若文件为写入方式打开，则把缓冲区内容写入文件
	if (file_handler->open_status == 2 && file_handler->point != -1) {
		fwrite(file_handler->buffer, file_handler->point, 1, file_handler->file);
	}
	//关闭文件并释放空间
	fclose(file_handler->file);
	free(file_handler->buffer);
	file_handler->open_status = 0;
	free(file_handler);
	return 1;
}



//int n = 0;
//FileHandler* file;
//char* test_str = NULL;
//file = open_fileb("dict.txt", "r");
//while (read_line(file, &test_str)) {
//	n++;
//	if (n % 10000 == 0)
//		printf("%d %s\n", n, test_str);
//}
//system("pause");
//return 0;

//
//  yn.h
//  hashtable_search
//
//  Created by 杨恩 on 2020/9/22.
//

#ifndef yn_h
#define yn_h

int get_hash(char* str);
int write_line(FileHandler* file_handler, char* str);

#endif /* yn_h */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#ifndef	node_struct
#define node_struct
#include "hash.c"

// SDBMHash�㷨
unsigned int SDBMHash(char* str);

// RS Hash�㷨
unsigned int RSHash(char* str);

// JS Hash�㷨
unsigned int JSHash(char* str);

// P.J.Weinberger Hash�㷨
unsigned int PJWHash(char* str);

// ELF Hash�㷨
unsigned int ELFHash(char* str);

// BKDR Hash�㷨
unsigned int BKDRHash(char* str);

// DJB Hash�㷨
unsigned int DJBHash(char* str);

// AP Hash�㷨
unsigned int APHash(char* str);

#endif
/*************************************************
����hash�㷨
��Դ��https://blog.csdn.net/kevin_darkelf/article/details/79724986
*************************************************/


// SDBMHash�㷨
unsigned int SDBMHash(char* str) {
	unsigned int hash = 0;
	while (*str)
	{
		// equivalent to: hash = 65599*hash + (*str++);
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	}
	return (hash & 0x7FFFFFFF);
}


// RS Hash�㷨
unsigned int RSHash(char* str) {
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	while (*str)
	{
		hash = hash * a + (*str++);
		a *= b;
	}
	return (hash & 0x7FFFFFFF);
}


// JS Hash�㷨
unsigned int JSHash(char* str) {
	unsigned int hash = 1315423911;
	while (*str)
	{
		hash ^= ((hash << 5) + (*str++) + (hash >> 2));
	}
	return (hash & 0x7FFFFFFF);
}



// P.J.Weinberger Hash�㷨
unsigned int PJWHash(char* str) {
	unsigned int BitsInUnignedInt = (unsigned int)(sizeof(unsigned int) * 8);
	unsigned int ThreeQuarters = (unsigned int)((BitsInUnignedInt * 3) / 4);
	unsigned int OneEighth = (unsigned int)(BitsInUnignedInt / 8);
	unsigned int HighBits = (unsigned int)(0xFFFFFFFF) << (BitsInUnignedInt - OneEighth);
	unsigned int hash = 0;
	unsigned int test = 0;
	while (*str)
	{
		hash = (hash << OneEighth) + (*str++);
		if ((test = hash & HighBits) != 0)
		{
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}
	return (hash & 0x7FFFFFFF);
}



// ELF Hash�㷨
unsigned int ELFHash(char* str) {
	unsigned int hash = 0;
	unsigned int x = 0;
	while (*str)
	{
		hash = (hash << 4) + (*str++);
		if ((x = hash & 0xF0000000L) != 0)
		{
			hash ^= (x >> 24);             hash &= ~x;
		}
	}
	return (hash & 0x7FFFFFFF);
}


// BKDR Hash�㷨
unsigned int BKDRHash(char* str) {
	unsigned int seed = 31; // 31 131 1313 13131 131313 etc..2^n-1
	unsigned int hash = 0;
	while (*str)
	{

		hash = hash * seed + (*str++);
	}
	return (hash & 0x7FFFFFFF);
}



// DJB Hash�㷨
unsigned int DJBHash(char* str) {
	unsigned int hash = 5381;
	while (*str)
	{
		hash += (hash << 5) + (*str++);
	}
	return (hash & 0x7FFFFFFF);
}



// AP Hash�㷨
unsigned int APHash(char* str) {
	unsigned int hash = 0;
	int i;
	for (i = 0; *str; i++)
	{
		if ((i & 1) == 0)
		{
			hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
		}
		else
		{
			hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
		}
	}
	return (hash & 0x7FFFFFFF);
}

#include <math.h>


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
	sprintf(target, "%d", num);

	while (*t != 0) {
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = *t;
		t++;
	}
	num++;

	//hang:
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 0xD0;
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 0xD0;
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 0x3A;
	//���ַ���д�뻺������

	while (*str != 0) {

		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = *str;
		str++;
	}
	//���뻻�з�
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 10;

	return 0;


}


#include <unistd.h>

//#char input_dict[99] = "/home/wanglei/course/";
//#char output_res[99] = "/home/wanglei/course/result/";


/* 变量的初始化 */
clock_t start, end;		//时间变量，用于统计运行时间
HashNode hash_table[23767] = { "",NULL };	//主Hash表
char list[256][256][256] = { 0 };	//一个三维数组，通过对比s_file字符串的前三个字节达到粗过滤，默认全为0
FileHandler* string_file;	//string.txt file handler
FileHandler* dict_file;		//dict.txt file handler	
FileHandler* result_file;	//result.txt file handler
char* str;


int main() {

	/* 打开用到的文件 */
	start = clock();
	string_file = open_file("/home/wanglei/course/string.txt", "r");
	dict_file = open_file("/home/wanglei/course/dict.txt", "r");
	result_file = open_file("/home/wanglei/course/result/result_37.txt", "w");
	end = clock();
	//printf("File Opened successfully. It takes %f seconds.\n", (float)(end - start) / CLOCKS_PER_SEC);

	/* 根据string.txt构建hash表 */
	//printf("Building the hash table now...\n");
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
	//printf("Construction of Hash Completed, it takes %f seconds, conflicts occurred %d times\n", (float)(start - end) / CLOCKS_PER_SEC, ct_num);

	/* 开始搜索，依次取dict.txt的每一行，在构建好的hash表中搜索 */
	//printf("Start Searching...\n");
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
				//？怎么判断 失败，失败后的具体操作有什么？
			}
		}
	}
	end = clock();
	//printf("Searching Completed. It takes %f seconds. \n", (float)(end - start) / CLOCKS_PER_SEC);//搜索完成，共用时%f秒

	/* 搜索任务完成，显示搜索结果并退出 */
	//printf("Task Completed. It is included %d lines. %d lines remaining after first filtering, %d lines have been found.\n", total_line, hash_line, success_line);//任务完成，一共 %d 行，首次过滤后剩余 %d 行，搜索到 %d 行。
	printf("runtime:%f      sring_march:%d\n", (float)(end - start) / CLOCKS_PER_SEC, success_line);

	close_file(string_file);
	close_file(dict_file);
	close_file(result_file);
	sleep(3);
	return 0;
}
