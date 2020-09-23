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
#include "struct.h"

unsigned int FILE_BUFFER_SIZE = 1048576;	//文件缓冲区大小，单位字节

/* 文件操作Handler,增加文件缓冲区加快读写速度 */
typedef struct FileHandler {
	FILE* file;		//被操作的文件指针
	char file_name[99];		//文件名
	int open_status;		//打开状态，为0时为关闭，1为read模式，2write模式
	int point;//指向在当前缓冲区中的读写位置
	unsigned int buffer_size;//文件缓冲区的大小
	char* buffer;//文件缓冲区
}FileHandler;


/*
* 函数名称：open_file
* 函数功能：负责打开文件
* 参数：	file_name：要打开文件的名称
*		open_type：文件的操作类型，如"w"
* 返回值：对应文件的指针
*/
FILE* open_file(char* file_name, char* open_type) {
	FILE* file = (FILE*)malloc(sizeof(FILE));
	file = fopen(file_name, open_type);		//打开对于文件
	if (!file) {	//若打开失败则显示错误信息并退出程序
		printf("open %s fail!", file_name);
		system("stop");
	}
	else {
		return file;
	}
	return file;
}

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
		new_node->str[0] = '\0';
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
* 函数名称：open_fileb
* 函数功能：负责根据给定内容创建新的hash节点
* 参数：	str：新创建节点的文本内容
* 返回值：HashNode指针
*/
FileHandler* open_file_b(char* file_name, char* open_type, ...) {
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
		fclose(file_handler->file); return 0;	//分配内存失败
	}

	/* FileHandler属性初始化 */
	strcpy(file_handler->file_name, file_name);
	file_handler->open_status = (open_type == "r" ? 1 : 2);
	file_handler->point = -1;
	return file_handler;
}

/*
* 函数名称：create_new_node
* 函数功能：负责根据给定内容创建新的hash节点
* 参数：	str：新创建节点的文本内容
* 返回值：HashNode指针
*/
int read_line(FileHandler* file_handler, char** str) {
	//TODO:已知bug：若文件结尾为一空行则不会输出
	if (file_handler->open_status != 1) return 0;	//打开模式为写入时不允许读取
	int old_point = file_handler->point;
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
					//读取到的整个buffer内容少于一行，则超过最大处理大小
					system("stop");
					return 0;
				}
			}
			*(file_handler->buffer + file_handler->point) = 0;//断行
			(*str) = &(file_handler->buffer[0]);
			return 1;
		}
		file_handler->point++;
	} while (*(file_handler->buffer + file_handler->point) != '\n');
	*(file_handler->buffer + file_handler->point) = 0;//断行
	(*str) = file_handler->buffer + old_point + 1;
	return 1;
}

/*
* 函数名称：create_new_node
* 函数功能：负责根据给定内容创建新的hash节点
* 参数：	str：新创建节点的文本内容
* 返回值：HashNode指针
*/
int write_line(FileHandler* file_handler, char* str) {
	return 0;
}
int close_file(FileHandler* file_handler) {
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