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
#include "gy.h"

int FILE_BUFFER_SIZE = 30485760;    //文件缓冲区大小，单位字节

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
* TODO:du read dict.txt fail when new buffer。
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
/*r
* 函数名称：manipulate_m
* 函数功能：读取与写入bloom过滤器
* 参数：    char*m：操作空间；
 unsigned int k： 哈希值
* 返回值：int,
*/
int manipulate_m(char*m,unsigned int h,int typ)
{//mzhizhen zijie
    if(typ==1)//写
    {
    unsigned char b=(char)128;//1000,0000永远只有1位是1
        b=b>>(h%8);
        *(m+h/8)=*(m+h/8)|b;//
    }
    if(typ==0)//读，判断是0是1;&&b
    {
        unsigned char b=(char)128;
        b=b>>(h%8);//0001,0000假如m+k/8为a,若a为0101，0000。b=0001,0000;0001,0000
        if(!(*(m+h/8)&b))
            return 0;//0则跳过这一行
    }
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

