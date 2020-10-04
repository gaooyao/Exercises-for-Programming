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
int n=1270574;//待存储的字符串个数

int main() {
    start = clock();
    char *new_m1;
    int m1,k1;
    float fp1=0.0001;//期望的错误率
    int num1=0;
    m1=num_m(fp1,n);//m位
    //printf("需要%d位的连续存储空间\n",m);
    k1=num_k(m1,n);//k个哈希函数
    //printf("需要Hash函数%d个\n",k);
    new_m1=create_m(m1);//连续的内存位置
    uint64_t* out[2];
	/* 打开dict.txt，根据dict.txt建立内存 */
    dict_file = open_file("dict.txt", "r");
    result_file = open_file("result.txt", "w");
    string_file = open_file("string.txt", "r");
    int tt = 0;
	while (read_line(dict_file, &str) == 1) {
		//tt++;
		//printf("dict%d\n", tt);
		for (int seed = 0; seed < 24; seed++) {//一行计算24次
			MurmurHash3_x64_128(str, strlen(str), seed, out);//计算哈希值
			manipulate_m(new_m1, (unsigned int)out[1] % m1, 1);//置0置1
		}//
	}
	/* 根据dict.txt建立内存结束 */
	/* 打开string.txt，根据string.txt查询 */
    tt = 0;
    while (read_line(string_file, &str) == 1){
        tt++;
        int t=0;//
        for(int seed=0;seed<24;seed++){
            if(t==0){
            MurmurHash3_x64_128(str, strlen(str),seed,out);
            if(!(manipulate_m(new_m1,(unsigned int)out[1]%m1,0)))
                t=1;
            }
        }
        
        if(t==0)
        {
            num1++;
            write_line(result_file, str);
        }

    }
    printf("Error rate:%4f\b string match:%d\b ",fp1,num1);
    printf("\n");
    end = clock();
    printf("程序执行时间为 %f 秒.\n", (float)(end - start) / CLOCKS_PER_SEC);
	/* 根据string.txt查询结束 */
	/* 关闭文件，结束程序 */
	close_file(dict_file);
    close_file(string_file);
    close_file(result_file);
    start = clock();
    char *new_m2;
    int num2=0;
    int m2,k2;
    float fp2=0.00001;//期望的错误率
    m2=num_m(fp2,n);//m位
    //printf("需要%d位的连续存储空间\n",m);
    k2=num_k(m2,n);//k个哈希函数
    //printf("需要Hash函数%d个\n",k);
    new_m2=create_m(m2);//连续的内存位置
    /* 打开dict.txt，根据dict.txt建立内存 */
    dict_file = open_file("dict.txt", "r");
    result_file = open_file("result.txt", "w");
    string_file = open_file("string.txt", "r");
    while (read_line(dict_file, &str) == 1) {
        for (int seed = 0; seed < 24; seed++) {//一行计算24次
            MurmurHash3_x64_128(str, strlen(str), seed, out);//计算哈希值
            manipulate_m(new_m2, (unsigned int)out[1] % m2, 1);//置0置1
        }//
    }
    /* 根据dict.txt建立内存结束 */
    /* 打开string.txt，根据string.txt查询 */
    while (read_line(string_file, &str) == 1){
        int t=0;//
        for(int seed=0;seed<24;seed++){
            if(t==0){
            MurmurHash3_x64_128(str, strlen(str),seed,out);
            if(!(manipulate_m(new_m2,(unsigned int)out[1]%m2,0)))
                t=1;
            }
        }
        if(t==0)
        {
           num2++;
            write_line(result_file, str);
        }
    }
    printf("Error rate:%5f\b string match:%d\b",fp2,num2);
    printf("\n");
    end = clock();
    printf("程序执行时间为 %f 秒.\n", (float)(end - start) / CLOCKS_PER_SEC);
    /* 根据string.txt查询结束 */
    /* 关闭文件，结束程序 */
    close_file(dict_file);
    close_file(string_file);
    close_file(result_file);
    char *new_m3;
    int m3,k3;
    int num3=0;
    float fp3=0.000001;
    start = clock();
    m3=num_m(fp3,n);//m位
    //printf("需要%d位的连续存储空间\n",m);
    k3=num_k(m3,n);//k个哈希函数
    //printf("需要Hash函数%d个\n",k);
    new_m3=create_m(m3);//连续的内存位置
    /* 打开dict.txt，根据dict.txt建立内存 */
    dict_file = open_file("dict.txt", "r");
    result_file = open_file("result.txt", "w");
    string_file = open_file("string.txt", "r");
    tt = 0;
    while (read_line(dict_file, &str) == 1) {
        //tt++;
        //printf("dict%d\n", tt);
        for (int seed = 0; seed < 24; seed++) {//一行计算24次
            MurmurHash3_x64_128(str, strlen(str), seed, out);//计算哈希值
            manipulate_m(new_m3, (unsigned int)out[1] % m3, 1);//置0置1
        }//
    }
    /* 根据dict.txt建立内存结束 */
    /* 打开string.txt，根据string.txt查询 */
    while (read_line(string_file, &str) == 1){
        int t=0;//
        for(int seed=0;seed<24;seed++){
            if(t==0){
            MurmurHash3_x64_128(str, strlen(str),seed,out);
            if(!(manipulate_m(new_m3,(unsigned int)out[1]%m3,0)))
                t=1;
            }
        }
        if(t==0){
            num3++;
            write_line(result_file, str);
        }
    }
    printf("Error rate:%6f\b string match:%d\b ",fp3,num3);
    printf("\n");
    end = clock();
    printf("程序执行时间为 %f 秒.\n", (float)(end - start) / CLOCKS_PER_SEC);
    /* 根据string.txt查询结束 */
    /* 关闭文件，结束程序 */
    close_file(dict_file);
    close_file(string_file);
    close_file(result_file);
    return 0;
    
	
}
