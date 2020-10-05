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
#include <unistd.h>
char input_dict[99] = "/home/wanglei/course/";
char output_dict[99] = "/home/wanglei/course/result/";

/* 变量的初始化 */
clock_t start, end;		//时间变量，用于统计运行时间
FileHandler* string_file;	//string.txt file handler
FileHandler* dict_file;		//dict.txt file handler	
FileHandler* result_file;	//result.txt file handler
char* str;
int n=1270574;//待存储的字符串个数
float fp_list[3] = { 0.0001, 0.00001, 0.000001 };
char result_file_name_list[3][99] = { "/home/wanglei/course/result/result_37_0.0001.txt", "/home/wanglei/course/result/result_37_0.00001.txt", "/home/wanglei/course/result/result_37_0.000001.txt" };

int main(){
    /* 打开dict.txt，根据dict.txt建立内存 */
    dict_file = open_file("/home/wanglei/course/dict.txt", "r");
    string_file = open_file("/home/wanglei/course/string.txt", "r");
    start=clock();
    int cycle_num;
    for (cycle_num = 0; cycle_num < 3; cycle_num++) {
        char* new_m;
        int m, k;
        float fp = fp_list[cycle_num];//期望的错误率
        int num = 0;
        m = num_m(fp, n);//m位
        //printf("需要%d位的连续存储空间\n",m);
        k = num_k(m, n);//k个哈希函数
        //printf("需要Hash函数%d个\n",k);
        new_m = create_m(m);//连续的内存位置
        uint64_t* out[2];

        result_file = open_file(result_file_name_list[cycle_num], "w");

        int tt = 0;
        while (read_line(dict_file, &str) == 1) {
            //tt++;
            //printf("dict%d\n", tt);

            for (int seed = 0; seed < 24; seed++) {//一行计算24次
                MurmurHash3_x64_128(str, strlen(str), seed, out);//计算哈希值
                manipulate_m(new_m, (unsigned int)out[1] % m, 1);//置0置1
            }//
        }
        /* 根据dict.txt建立内存结束 */
        /* 打开string.txt，根据string.txt查询 */
        tt = 0;
        while (read_line(string_file, &str) == 1) {
            tt++;
            int t = 0;
            for (int seed = 0; seed < 24; seed++) {
                if (t == 0) {
                    MurmurHash3_x64_128(str, strlen(str), seed, out);
                    if (!(manipulate_m(new_m, (unsigned int)out[1] % m, 0)))
                        t = 1;
                }
            }
            if (t == 0)
            {
                num++;
                write_line(result_file, str);
            }
        }
        printf("Error rate:%f string match:%d ", fp, num);
        printf("\n");
        //printf("use %f seconds.\n", (float)(end - start) / CLOCKS_PER_SEC);
        /* 根据string.txt查询结束 */
        /* 关闭文件，结束程序 */
        close_file(result_file);
        reset_file(dict_file);
        reset_file(string_file);
        
        
    }
    end=clock();
    close_file(dict_file);
    close_file(string_file);
    printf("runtime: %f s\n" ,(float)(end - start) / CLOCKS_PER_SEC);
    sleep(3);
    return 0;
}
