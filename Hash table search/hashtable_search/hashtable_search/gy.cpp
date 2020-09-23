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
#include <stdarg.h>
#include "struct.h"

#define FILE_BUFFER_SIZE = 1048576;

typedef struct FileHandler {
	FILE* file;
	char file_name[99];
	int open_status;		//值为0时为关闭状态，1为read模式，2write模式
	int point;
	unsigned int buffer_size;
	char* buffer;
	struct FileHandler* next;
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


FileHandler* open_fileb(char* file_name, char* open_type, ...) {
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
	va_list valist;
	va_start(valist, file_name, open_status);
	file_handler->buffer_size = va_arg(valist, int);
	file_handler->buffer_size = file_handler->buffer_size > 0 ? file_handler->buffer_size : 1048576;
	va_end(valist);
	/* FileHandler属性初始化 */
	file_handler->buffer = (char*)malloc(file_handler->buffer_size);
	if (!file_handler->buffer)fclose(file_handler->file); return 0;	//分配内存失败
	strcpy(file_handler->file_name, file_name);
	file_handler->open_status = (open_type == "r" ? 1 : 2);
	file_handler->point = -1;
	file_handler->next = NULL;
	return file_handler;
}

int read_line(FileHandler* file_handler, char* str) {
	if (file_handler->open_status !=1) return 0;	//打开模式为写入时不允许读取
	int old_point = file_handler->point;
	do {
		//如果已读到文件末尾则返回
		if (file_handler->point != -1 && file_handler->buffer[file_handler->point] == -1)return 0;

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
		if (file_handler->point + 1 > file_handler->buffer_size) {
			//copy buffer尾部数据至头部
			int cope_f = 0;
			while (old_point < file_handler->buffer_size) {
				file_handler->buffer[cope_f] = file_handler->buffer[old_point];
				cope_f++;
				old_point++;
			}
			//读入新数据
			int read_num;
			read_num = fread(file_handler->buffer + cope_f + 1, 1, file_handler->buffer_size - cope_f - 1, file_handler->file);
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
			while (file_handler->buffer[file_handler->point] && file_handler->buffer[file_handler->point] != -1) {
				file_handler->point++;
				if (file_handler->point > file_handler->buffer_size) {
					//读取到的整个buffer内容少于一行，则超过最大处理大小
					system("stop");
					return 0;
				}
			}
			str = &(file_handler->buffer[0]);
			return 1;
		}
		file_handler->point++;
	} while (file_handler->buffer[file_handler->point]);

	str = &(file_handler->buffer[old_point]);
	return 1;
}

int write_line(FileHandler* file_handler, char* str) {
	return 0;
}
int close_file(FileHandler* file_handler) {
	free(file_handler->buffer);
	file_handler->open_status = 0;
	free(file_handler);
	return 1;
}
