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
	}
}

static char str[99];
void get_line() {
	clock_t start, end;
	
	FILE* file;
	file = fopen("dict.txt", "r");
	start = clock();
	
	/*while (fscanf(file, "%s", str) != -1) {

	}*/

	char buffer[1048576];
	char* point;
	int num = 0;
	fread(buffer, 1, 1048576,file);
	point = buffer;
	while ((int)point!=-1) {
		printf("%s", point);

	}

	/*fscanf((FILE*)buffer, "%s", str);
	printf("%s", str);*/

	end = clock();
	printf("%f", (float)(end - start) / CLOCKS_PER_SEC);
	fclose(file);
}