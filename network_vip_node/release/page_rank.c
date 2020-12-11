
//网页结点
typedef struct page_node
{
    char *path;            //网页路径
    int out_link_num;      //出链数目
    float out_link_weight; //出链的权重
    int *out_link_list;    //出链列表
} PageNode;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

int int_len = sizeof(int);
int char_len = sizeof(char);

float page_rank_a = 0.15;
float page_rank_b = 0.85;
float page_rank_error = 0.0001;

int page_number = 0;                  //网页的总数目
PageNode **page_list;                 //存储网页结点信息
float average_weight;                 //全局平均权重，如A没有出链指向B，则A跳转到B的概率为average_weight（随机跳转机制），值为发生随机跳转的概率除以所以网页的数目
float *page_rank_old, *page_rank_new; //幂迭代计算的初始向量，计算后向量

double self_pow(double num1, int num2)
{
    double result = 1;
    int i;
    for (i = 0; i < num2; i++)
    {
        result *= num1;
    }
    return result;
}
float self_sqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;             // get bits for floating VALUE
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float *)&i;               // convert bits BACK to float
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    return 1 / x;
}

float *calc_new_rank(float *page_rank)
{
    PageNode *page;
    int i, j, k;
    float *new_rank = (float *)malloc(sizeof(float) * page_number);
    for (i = 0; i < page_number; i++)
    {
        new_rank[i] = average_weight;
    }
    for (i = 0; i < page_number; i++)
    {
        page = page_list[i];
        for (j = 0; j < page->out_link_num; j++)
        {
            new_rank[page->out_link_list[j]] += ((page_rank[i]) * (page->out_link_weight));
        }
    }
    free(page_rank);
    return new_rank;
}

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
函数说明：此函数调用各子函数，完成graph.bin构建任务
*/
void page_rank()
{
    printf("Start read graph.bin.\n");
    clock_t start_time, end_time;
    start_time = clock();
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
        page_list[i]->out_link_list = (int *)malloc(int_len * out_link_num);
        //对一个网页而言，其所有出链的权重是相同的，这里用out_link_weight存储这些出链的权重。对于此网页，它没有出链指向的网页的权重为average_weight
        page_list[i]->out_link_weight = (page_rank_b / out_link_num);
        //读取out_link_num个出链信息
        for (j = 0; j < out_link_num; j++)
        {
            fread(&out_link_page_id, int_len, 1, graph_bin);
            page_list[i]->out_link_list[j] = out_link_page_id;
        }
    }
    //构建幂迭代的初始向量，这里设为平均分布
    float temp_weight = (float)1 / page_number;
    page_rank_new = (float *)malloc(sizeof(float) * page_number);
    page_rank_old = (float *)malloc(sizeof(float) * page_number);
    for (i = 0; i < page_number; i++)
    {
        page_rank_new[i] = temp_weight;
    }
    //进行幂迭代计算，当误差小于page_rank_error时停止计算
    float error;
    int iteration_num = 0;
    do
    {
        iteration_num++;
        for (i = 0; i < page_number; i++)
        {
            page_rank_old[i] = page_rank_new[i];
        }
        page_rank_new = calc_new_rank(page_rank_new);
        error = calc_error(page_rank_old, page_rank_new);
        printf("The iteration %d completed, now the accuracy rate is %f.\n", iteration_num, error);
    } while (error > page_rank_error);
    //输出前20名
    FILE *result_file;
    result_file = fopen("page_rank_result.txt", "w");
    printf("Iteration number: %d.\n", iteration_num);
    fprintf(result_file, "Iteration number: %d.\n", iteration_num);
    float max_rank = 0;
    int highest_page_id;
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
        fprintf(result_file, "The %2d st's page rank number is %f, path is %s.\n", i + 1, page_rank_new[highest_page_id], page_list[highest_page_id]->path);
        page_rank_new[highest_page_id] = 0;
        max_rank = 0;
    }
    fclose(result_file);
    end_time = clock();
    printf("All finished, use time %f seconds.\n", (float)(end_time - start_time) / CLOCKS_PER_SEC);
}

#include <stdio.h>
#include <unistd.h>

int main()
{
    page_rank(); //page rank计算
    sleep(3);
    getchar();
    return 0;
}