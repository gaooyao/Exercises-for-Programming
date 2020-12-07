#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>

#include "brandes_between.h"

#define thread_num 8              //迭代时用的线程数目
char *bin_name = "graph.bin";     //graph文件名
pthread_t tid[thread_num];        //线程指针
ThreadArr thread_arr[thread_num]; //线程传参用的结构体
int int_len = sizeof(int);        //常用到int长度信息，在此算出减少计算量
int char_len = sizeof(char);      //常用到char长度信息，在此算出减少计算量
int page_number = 0;              //所有网页的数量
Page *page_list;                  //存储网页结点信息的队列
int new_visited_multiple = 1024;  //子线程数据中，存储新访问节点数组相对于总节点数的长度，因为某轮遍历中某个节点会被访问多次，因此有可能出现总新访问次数大于总节点数目，所有要加大存储新访问节点数组的容量。

/*
函数说明：各子线程运行的主函数，负责对一部分节点进行广度优先搜索，计算出最短路径相关信息
参数说明：
         thread_arr：线程用到的所有数据的结构体
*/
void *calc_ptah_num(ThreadArr *thread_arr)
{
    int i, j, k;
    thread_arr->all_path_num = 0;                                                          //此线程计算出的最短路径数量
    thread_arr->out_num = (int *)malloc(int_len * page_number);                            //存储每个网页经过它的最短路径数
    memset(thread_arr->out_num, 0, int_len * page_number);                                 //最短路径数初始化
    thread_arr->is_visited = (int *)malloc(int_len * page_number);                         //存储是否已经计算过i节点到一个节点的最短路径
    thread_arr->new_visited = (int *)malloc(int_len * page_number * new_visited_multiple); //缓存新访问的节点
    for (i = thread_arr->start_num; i <= thread_arr->end_num; i++)
    {
        printf("Thread %d all task %d, now calculating %d.\n", thread_arr->thread_id, thread_arr->all_node_num, i - thread_arr->start_num + 1);
        memset(thread_arr->is_visited, 0, int_len * page_number);                         //开始时设所有节点都没被访问过
        thread_arr->is_visited[i] = 1;                                                    //默认i节点已被访问过
        memset(thread_arr->new_visited, 0, int_len * page_number * new_visited_multiple); //新访问的节点数组初始化
        thread_arr->path_point = (Path *)malloc(sizeof(Path));                            //创建路径信息队列
        thread_arr->path_point->node_num = 1;
        thread_arr->path_point->node_list = (int *)malloc(int_len);
        thread_arr->path_point->node_list[0] = i;
        thread_arr->path_point->next = NULL;
        thread_arr->path_queue_tail = thread_arr->path_point; //路径信息队列的队尾
        thread_arr->old_path_num = 0;                         //遍历时，路径信息队列中旧路径的数目
        thread_arr->new_path_num = 1;                         //遍历时，路径信息队列中新路径的数目
        do
        {
            /*
            每轮遍历访问所有旧路经信息，找到每条老路径末尾节点能到达的新节点并以此创建新路径，
            直到某一轮遍历找不到任何新路径，则i节点能够到达的所有节点寻找完毕
        */
            thread_arr->old_path_num = thread_arr->new_path_num;
            thread_arr->new_path_num = 0;
            while (thread_arr->old_path_num) //遍历所有旧路经
            {
                thread_arr->tail_page = &page_list[thread_arr->path_point->node_list[thread_arr->path_point->node_num - 1]]; //取该路径上最后一个网页节点
                //遍历该网页节点的出链信息，若一个出链指向的节点未被访问过，则基于当前路径创建一条新路径
                for (j = 0; j < thread_arr->tail_page->link_num; j++)
                {
                    if (!thread_arr->is_visited[thread_arr->tail_page->link_list[j]]) //判断出链指向的节点是否被访问过
                    {
                        thread_arr->new_path = (Path *)malloc(sizeof(Path)); //未被访问，创建新路径
                        thread_arr->new_path->node_num = thread_arr->path_point->node_num + 1;
                        thread_arr->new_path->next = NULL;
                        thread_arr->new_path->node_list = (int *)malloc(int_len * thread_arr->new_path->node_num);
                        //创建新路径的路径信息为老路径信息加新访问的节点
                        memcpy(thread_arr->new_path->node_list, thread_arr->path_point->node_list, thread_arr->path_point->node_num * int_len);
                        thread_arr->new_path->node_list[thread_arr->new_path->node_num - 1] = thread_arr->tail_page->link_list[j];
                        //创建的新路径加入路径信息队列
                        thread_arr->path_queue_tail->next = thread_arr->new_path;
                        thread_arr->path_queue_tail = thread_arr->new_path;
                        //若新路径长度大于2，则总路径数加一，经过的节点的被经过数加一，若小于2则说明路径没有经过中间节点，则不增加
                        if (thread_arr->new_path->node_num > 2)
                        {
                            //总路径数加一
                            thread_arr->all_path_num++;
                            //新路径上所有新经过的节点它们的被经过路径数加一
                            for (k = 1; k < thread_arr->new_path->node_num - 1; k++)
                            {
                                thread_arr->out_num[thread_arr->new_path->node_list[k]]++;
                            }
                        }
                        //新被访问的节点加入新访问节点队列
                        thread_arr->new_visited[thread_arr->new_path_num] = thread_arr->tail_page->link_list[j];
                        thread_arr->new_path_num++;
                    }
                }
                //释放老路径节点
                thread_arr->temp_path = thread_arr->path_point;
                thread_arr->path_point = thread_arr->path_point->next;
                free(thread_arr->temp_path->node_list);
                free(thread_arr->temp_path);
                thread_arr->old_path_num--;
            }
            //遍历完老路径之后，把所有新被访问到的节点放入已被访问节点列表
            for (j = 0; j < thread_arr->new_path_num; j++)
            {
                thread_arr->is_visited[thread_arr->new_visited[j]] = 1;
            }
        } while (thread_arr->new_path_num);
    }
}

//主函数
void brandes_between()
{
    printf("Start read graph.bin.\n");
    //打开graph.bin
    FILE *graph_bin;
    if ((graph_bin = fopen(bin_name, "rb")) == NULL)
    {
        printf("Open graph.bin fail!");
        return;
    }
    //读取网页结点数目
    fread(&page_number, int_len, 1, graph_bin);
    printf("Page number: %d.\n", page_number);
    //初始化存储所有网页结点信息的page_list
    page_list = (Page *)malloc(sizeof(Page) * page_number);
    int i, j, k, out_link_num //临时存放一个网页的出链数目
        ,
        out_link_page_id; //临时存放一个出链的指向
    char temp_char;
    char temp_page_path[4096]; //临时存放网页路径信息
    //读取所以网页出链信息
    for (i = 0; i < page_number; i++)
    {
        //读取网页路径
        k = 0;
        do
        {
            fread(&temp_char, char_len, 1, graph_bin);
            temp_page_path[k] = temp_char;
            k++;
        } while (temp_char);
        temp_page_path[k] = 0;
        //生成新的PageNode结点并初始化
        page_list[i].path = (char *)malloc(strlen(temp_page_path) + 1);
        strcpy(page_list[i].path, temp_page_path);
        //读取此网页的出链数目
        fread(&out_link_num, int_len, 1, graph_bin);
        page_list[i].link_num = out_link_num;
        //构建存储此网页出链指向信息的数组
        page_list[i].link_list = (int *)malloc(int_len * out_link_num);
        //读取出链信息
        for (j = 0; j < out_link_num; j++)
        {
            fread(&out_link_page_id, int_len, 1, graph_bin);
            page_list[i].link_list[j] = out_link_page_id;
        }
    }
    printf("Begin calculate, use %d thread.\n", thread_num);
    //创建多线程，每个线程负责计算一部分网页的介数
    for (int i = 0; i < thread_num; i++)
    {
        thread_arr[i].thread_id = i + 1;
        //设置该线程需要计算的节点范围
        thread_arr[i].start_num = page_number / thread_num * i;
        thread_arr[i].end_num = page_number / thread_num * (i + 1) - 1;
        if (i == thread_num - 1)
        {
            //最后一个子线程需计算到最后一个网页节点
            thread_arr[i].end_num = page_number - 1;
        }
        thread_arr[i].all_node_num = thread_arr[i].end_num - thread_arr[i].start_num + 1;
        //创建线程，开始计算
        pthread_create(&tid[i], NULL, (void *)calc_ptah_num, &thread_arr[i]);
    }
    //回收各子线程，确保都计算完后再执行后面的语句
    for (int i = 0; i < thread_num; i++)
    {
        pthread_join(tid[i], NULL); //等待线程结束
    }
    //各线程得到的信息汇总
    unsigned long all_path_num = 0;                               //总的路径数量
    unsigned int *out_num = (int *)malloc(int_len * page_number); //每个网页经过它的最短路径数总和
    memset(out_num, 0, int_len * page_number);
    for (i = 0; i < thread_num; i++)
    {
        all_path_num += thread_arr[i].all_path_num; //把该线程计算出的最短路径数量加到总的路径数量
        for (j = 0; j < page_number; j++)
        {
            out_num[j] += thread_arr[i].out_num[j]; //把该线程计算出的每个节点被经过路径数量加到它的总路径数量上
        }
    }
    //输出被经过次数最多的前20个网页
    FILE *result_file;
    result_file = fopen("brandes_between_result.txt", "w");
    printf("All path number is %ld.\n", all_path_num);
    fprintf(result_file, "All path number is %ld.\n", all_path_num);
    float max_value = 0;
    int highest_page_id;
    for (i = 0; i < 20; i++) //每轮循环找到一个被经过次数最多的节点
    {
        for (j = 0; j < page_number; j++)
        {
            if (out_num[j] > max_value)
            {
                max_value = out_num[j];
                highest_page_id = j;
            }
        }
        //输出到文件
        printf("The %2dst's page's passing paths is %u, centrality is %f, path is %s.\n", i + 1, out_num[highest_page_id], ((double)max_value) / all_path_num, page_list[highest_page_id].path);
        fprintf(result_file, "The %2dst's page's passing paths is %u, centrality is %f, path is %s.\n", i + 1, out_num[highest_page_id], ((double)max_value) / all_path_num, page_list[highest_page_id].path);
        out_num[highest_page_id] = 0;
        max_value = 0;
    }
    fclose(result_file);
    return;
}