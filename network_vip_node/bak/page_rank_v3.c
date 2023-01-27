#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>

#include "string_file_reader.h"
#include "config.h"
#include "page_rank.h"

#define thread_num 8 //迭代时用的线程数目

pthread_t tid[thread_num];        //线程指针
ThreadArr thread_arr[thread_num]; //辅助传参的结构体
float page_rank_a = 0.15;         //发生随机跳转概率
float page_rank_b = 0.85;         //不发生随机跳转概率
float accuracy_rate = 0.001;      //rank精确度要求

int page_number = 0;                  //网页的总数目
PageNode **page_list;                 //一个数组，存储网页的出链信息
LinkNode *link_list;                  //一个数组，存储链接的网页信息，由page_list转换而来
float average_weight;                 //全局平均权重，如A没有出链指向B，则A跳转到B的概率为average_weight（随机跳转机制），值为发生随机跳转的概率除以所以网页的数目
float *page_rank_old, *page_rank_new; //幂迭代计算的初始向量，计算后向量

/*
函数说明：计算num1的num2次方
*/
double self_pow(double num1, int num2)
{
    double result = 1;
    int i;
    for (i = 0; i < num2; i++)
        result *= num1;
    return result;
}
/*
函数说明：计算平方根函数
*/
float self_sqrt(float x)
{
    float xhalf = 0.5f * x;
    long i;
    i = *(long *)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);
    x = x * (1.5f - xhalf * x * x);
    x = x * (1.5f - xhalf * x * x);
    return 1 / x;
}

/*
函数说明：快速排序算法
*/
void quick_sort(int *arr, int begin, int end)
{
    int temp;
    if (begin < end)
    {
        temp = arr[begin];
        int i = begin;
        int j = end;
        while (i < j)
        {
            while (i < j && arr[j] >= temp)
            {
                j--;
            }
            arr[i] = arr[j];
            while (i < j && arr[i] <= temp)
            {
                i++;
            }
            arr[j] = arr[i];
        }
        arr[i] = temp;
        quick_sort(arr, begin, i - 1);
        quick_sort(arr, i + 1, end);
    }
}

/*
函数说明：各子线程运行的主函数，负责计算rank与转移概率矩阵的若干列相乘
参数说明：
         thread_arr：包含计算时用到的新旧rank，开始结束列的相关数据
*/
void *calc_rank(ThreadArr *thread_arr)
{
    PageNode *page;
    LinkNode *self_link_node;
    int i, j, k, l;
    float sum;
    //从转移概率矩阵的开始列，到结束列，分别计算与rank值的乘积
    for (i = thread_arr->start_num; i < thread_arr->end_num; i++)
    {
        self_link_node = &link_list[i];
        sum = 0;
        self_link_node->next_page_index = 0;
        //把rank与矩阵对应列相乘
        for (j = 0; j < self_link_node->page_num - 1; j++)
        {
            //page_list[j].page_id + 1到page_list[j + 1].page_id区间的节点的权重为全局平均权重average_weight
            for (k = self_link_node->page_list[j].page_id + 1; k < self_link_node->page_list[j + 1].page_id; k++)
            {
                sum += (average_weight * thread_arr->source_rank[k]);
            }
            //关键点权重从page_list[j + 1]中取
            sum += (self_link_node->page_list[j + 1].page_weight * thread_arr->source_rank[k]);
        }
        thread_arr->dis_rank[i] = sum;
    }
}

/*
函数说明：计算pagerank主函数，把当前rank与出链节点信息矩阵相乘，得到新的rank值
参数说明：
         page_rank：当前rank排名
*/
float *calc_new_rank(float *page_rank)
{
    PageNode *page;
    LinkNode *self_link_node;
    int i, j, k, l;
    float sum;
    //创建存储新rank的数组
    float *new_rank = (float *)malloc(sizeof(float) * page_number);
    //把转移概率矩阵分成thread_num块，分给各子线程运行
    for (int l = 0; l < thread_num; l++)
    {
        //构造子线程的参数，主要是开始列start_num和结束列end_num
        thread_arr[l].source_rank = page_rank;
        thread_arr[l].dis_rank = new_rank;
        thread_arr[l].start_num = page_number / thread_num * l;
        thread_arr[l].end_num = page_number / thread_num * (l + 1) - 1;
        if (l == thread_num - 1)
        {
            //最后一个子线程只需计算到最后一列
            thread_arr[l].end_num = page_number - 1;
        }
        //创建线程开始计算
        pthread_create(&tid[l], NULL, (void *)calc_rank, &thread_arr[l]);
    }
    //回收各子线程，确保都计算完后再执行后面的语句
    for (int l = 0; l < thread_num; l++)
    {
        pthread_join(tid[l], NULL); //等待线程结束
    }
    //计算完毕，返回新rank值
    return new_rank;
}

/*
函数说明：计算错误率
参数说明：
         page_rank_old：旧的rank排名
         page_rank_new：新的rank排名
*/
float calc_error(float *page_rank_old, float *page_rank_new)
{
    float error = 0;
    int i;
    for (i = 0; i < page_number; i++)
    {
        error += (self_pow(page_rank_new[i] - page_rank_old[i], 2));
    }
    return self_sqrt(error);
}
/*
函数说明：page_rank主函数，此函数调用各子函数，完成网页排名任务
*/
void page_rank_v3()
{
    printf("Start read graph.bin.\n");
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    FILE *graph_bin;
    //打开graph.bin
    if ((graph_bin = fopen("graph.bin", "rb")) == NULL)
    {
        printf("Open graph.bin fail!");
        return;
    }
    //读取网页结点数目
    fread(&page_number, int_len, 1, graph_bin);
    //计算全局平均权重
    average_weight = page_rank_a / page_number;
    printf("Page number: %d.\n", page_number);
    //初始化存储所有网页结点信息的page_list
    page_list = (PageNode **)malloc(sizeof(PageNode *) * page_number);
    int i, j, k, out_link_num //临时存放一个网页的出链数目
        ,
        out_link_page_id; //临时存放一个出链的指向
    char temp_char;
    char temp_page_path[4096]; //临时存放网页路径信息
    //读取所以网页出链信息
    printf("Start read page out link.\n");
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
        page_list[i] = (PageNode *)malloc(sizeof(PageNode));
        page_list[i]->path = (char *)malloc(strlen(temp_page_path) + 1);
        strcpy(page_list[i]->path, temp_page_path);
        //读取此网页的出链数目
        fread(&out_link_num, int_len, 1, graph_bin);
        page_list[i]->out_link_num = out_link_num;
        //构建存储此网页出链指向信息的数组
        page_list[i]->out_link_list = (int *)malloc(sizeof(int) * out_link_num + 1);
        //对一个网页而言，其所有出链的权重是相同的，这里用out_link_weight存储这些出链的权重。对于此网页，它没有出链指向的网页的权重为average_weight
        page_list[i]->out_link_weight = ((float)1 / out_link_num) * page_rank_b + average_weight;
        //读取out_link_num个出链信息
        for (j = 0; j < out_link_num; j++)
        {
            fread(&out_link_page_id, int_len, 1, graph_bin);
            page_list[i]->out_link_list[j] = out_link_page_id;
        }
        page_list[i]->out_link_list[out_link_num] = INT_MAX; //多存一个int最大值，为了加速矩阵计算
        //对这个网页的出链的id从小到大排序，为后面幂迭代时计算方便
        quick_sort(page_list[i]->out_link_list, 0, out_link_num - 1);
    }
    printf("Read page information finished.\n");
    //构建竖排矩阵，由网页出链矩阵转换而来
    printf("Start build link matrix.\n");
    gettimeofday(&end_time, NULL);
    //构建竖排矩阵的头节点
    link_list = (LinkNode *)malloc(sizeof(LinkNode) * page_number + 1);
    //存放找出来的临时出链网页信息
    PageWeight *temp_list = (PageWeight *)malloc(sizeof(PageWeight) * page_number);
    //遍历网页结点数组前先初始化其遍历相关参数
    for (i = 0; i < page_number; i++)
    {
        page_list[i]->next_out_index = 0;
        page_list[i]->next_out_id = page_list[i]->out_link_list[page_list[i]->next_out_index];
    }
    //开始构建链接结点数组
    for (i = 0; i < page_number; i++)
    {
        int self_page_num = 0; //有以此链接节点为出链的网页的数目
        for (j = 0; j < page_number; j++)
        {
            //根据next_out_id是知道下一个出链的id的，其权值不同于全局平均权重，在这里构建链接结点数组时只存放不同于全局权重的节点的信息
            if (i == page_list[j]->next_out_id)
            {
                //找到下一个出链id
                page_list[j]->next_out_id = page_list[j]->out_link_list[++page_list[j]->next_out_index];
                //此网页的出链信息存放到临时出链数组中
                temp_list[self_page_num].page_id = j;
                temp_list[self_page_num].page_weight = page_list[j]->out_link_weight;
                self_page_num++; //网页节点数目加一
            }
        }
        /*
        出链节点的网页id数组page_list长度为实际网页数目加2。
        设实际数目2，网页id为333，666，则page_list为[-1, 333, 666, page_number]，
        里面包含三个区间和四个关键点-1,(0-332),333,(334-665),666,(667-page_number)，
        关键点的权重从page_list中取，三个区间上各位置的权重为全局权重。
        通过这种转换，把以网页为单位的横排结构的矩阵转换为以链接信息为单位的竖排的矩阵结构，且采用存储区间关键点的这种方式能够在计算时减少if判断。
        转换前计算一轮rank平均需要185秒，转换后需60秒，提速308%。
        */
        self_page_num += 2; //page_list长度为实际数目加2
        link_list[i].page_num = self_page_num;
        link_list[i].next_page_index = 0; //存储遍历时此LinkNode的下一个关键点位置
        link_list[i].page_list = (PageWeight *)malloc(sizeof(PageWeight) * self_page_num + 1);
        link_list[i].page_list[0].page_id = -1;                          //关键点-1
        link_list[i].page_list[0].page_weight = 0;                       //权重为0，此关键点参与计算但不会影响计算结果
        link_list[i].page_list[self_page_num - 1].page_id = page_number; //关键点page_number
        link_list[i].page_list[self_page_num - 1].page_weight = 0;       //权重为0，此关键点参与计算但不会影响计算结果
        //把遍历出的实际关键点temp_list放入此link_list数组
        for (k = 0; k < self_page_num - 2; k++)
        {
            link_list[i].page_list[k + 1].page_id = temp_list[k].page_id;
            link_list[i].page_list[k + 1].page_weight = temp_list[k].page_weight;
        }
        if (!(i % 10000))
        {
            printf("ALL number %d, finished number: %6d.\n", page_number, i);
        }
    }

    free(temp_list); //释放临时结点数组
    gettimeofday(&end_time, NULL);
    printf("Build link matrix finished, use time %.3f seconds.\n", (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0);
    //构建幂迭代的初始向量，这里设为平均分布
    float temp_weight = (float)1 / page_number;
    page_rank_new = (float *)malloc(sizeof(float) * page_number);
    page_rank_old = (float *)malloc(sizeof(float) * page_number);
    for (i = 0; i < page_number; i++)
    {
        page_rank_new[i] = temp_weight;
    }
    //进行幂迭代计算，当误差小于accuracy_rate时停止计算
    float error;
    int iteration_num = 0;
    struct timeval iteration_start, iteration_end;
    do
    {
        gettimeofday(&iteration_start, NULL);
        iteration_num++; //迭代次数加一
        printf("Start iteration %d.\n", iteration_num);
        //存储旧的rank信息
        for (i = 0; i < page_number; i++)
        {
            page_rank_old[i] = page_rank_new[i];
        }
        //计算新的rank
        page_rank_new = calc_new_rank(page_rank_new);
        //计算新rank的精确度
        error = calc_error(page_rank_old, page_rank_new);
        gettimeofday(&iteration_end, NULL);
        printf("The iteration %d completed, now the accuracy rate is %f, use time %.3f seconds.\n", iteration_num, error, (iteration_end.tv_sec - iteration_start.tv_sec) + (iteration_end.tv_usec - iteration_start.tv_usec) / 1000000.0);
    } while (error > accuracy_rate); //当精确度小于要求时停止迭代
    //输出到文件
    FILE *result_file;
    result_file = fopen("page_rank_result.txt", "w");
    //输出迭代次数
    gettimeofday(&end_time, NULL);
    printf("Accuracy rate: %f, iteration number: %d, all time use %.3f seconds.\n", accuracy_rate, iteration_num, (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0);
    fprintf(result_file, "Accuracy rate: %f, iteration number: %d, all time use %.3f seconds.\n", accuracy_rate, iteration_num, (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0);
    float max_rank = 0;
    int highest_page_id;
    //输出rank前20名
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < page_number; j++)
        {
            if (page_rank_new[j] > max_rank)
            {
                max_rank = page_rank_new[j];
                highest_page_id = j;
            }
        }
        printf("The %2dst's page rank number is %f, path is %s.\n", i + 1, page_rank_new[highest_page_id], page_list[highest_page_id]->path);
        fprintf(result_file, "The %2dst's page rank number is %f, path is %s.\n", i + 1, page_rank_new[highest_page_id], page_list[highest_page_id]->path);
        page_rank_new[highest_page_id] = 0;
        max_rank = 0;
    }
    //rank计算结束
    fclose(result_file);
    printf("Generate graph.bin finished, use time %.3f seconds.\n", (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0);
}
