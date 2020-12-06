#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "brandes_between.h"

int int_len = sizeof(int);
int char_len = sizeof(char);
int page_number = 0;
unsigned long all_path_num = 0;

Page *page_list; //存储网页结点信息

void brandes_between()
{
    printf("Start read graph.bin.\n");
    clock_t start_time, end_time;
    start_time = clock();
    FILE *graph_bin;
    //打开graph.bin
    if ((graph_bin = fopen("graph_brandes_between.bin", "rb")) == NULL)
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
    unsigned int *out_num = (int *)malloc(int_len * page_number);
    memset(out_num, 0, int_len * page_number);

    int *is_visited = (int *)malloc(int_len * page_number);
    int *new_visited = (int *)malloc(int_len * page_number * 1024);

    for (i = 0; i < page_number; i++)
    {
        printf("Calc node %d.\n", i);
        memset(is_visited, 0, int_len * page_number);
        is_visited[i] = 1;
        memset(new_visited, 0, int_len * page_number);

        Path *path_point = (Path *)malloc(sizeof(Path));
        path_point->node_num = 1;
        path_point->node_list = (int *)malloc(int_len);
        path_point->node_list[0] = i;
        path_point->next = NULL;
        Path *path_queue_tail = path_point;
        int old_path_num = 0;
        int new_path_num = 1;
        do
        {
            old_path_num = new_path_num;
            new_path_num = 0;
            while (old_path_num)
            {
                Page *tail_page = &page_list[path_point->node_list[path_point->node_num - 1]];
                for (j = 0; j < tail_page->link_num; j++)
                {
                    if (!is_visited[tail_page->link_list[j]])
                    {
                        Path *new_path = (Path *)malloc(sizeof(Path));
                        new_path->node_num = path_point->node_num + 1;
                        new_path->next = NULL;
                        new_path->node_list = (int *)malloc(int_len * new_path->node_num);
                        for (k = 0; k < path_point->node_num; k++)
                        {
                            new_path->node_list[k] = path_point->node_list[k];
                        }
                        new_path->node_list[new_path->node_num - 1] = tail_page->link_list[j];
                        path_queue_tail->next = new_path;
                        path_queue_tail = new_path;
                        for (k = 1; k < new_path->node_num - 1; k++)
                        {
                            out_num[new_path->node_list[k]]++;
                            all_path_num++;
                        }
                        new_visited[new_path_num] = tail_page->link_list[j];
                        new_path_num++;
                    }
                }
                Path *temp_path = path_point;
                path_point = path_point->next;
                free(temp_path->node_list);
                free(temp_path);
                old_path_num--;
            }
            for (j = 0; j < new_path_num; j++)
            {
                is_visited[new_visited[j]] = 1;
            }
        } while (new_path_num);
    }

    //输出前20名
    FILE *result_file;
    result_file = fopen("brandes_between_result.txt", "w");
    printf("All path number is %ld.\n", all_path_num);
    fprintf(result_file, "All path number is %ld.\n", all_path_num);
    float max_value = 0;
    int highest_page_id;
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < page_number; j++)
        {
            if (out_num[j] > max_value)
            {
                max_value = out_num[j];
                highest_page_id = j;
            }
        }
        printf("The %2dst's page's passing paths is %u, centrality is %f, path is %s.\n", i + 1, out_num[highest_page_id], ((double)max_value) / all_path_num, page_list[highest_page_id].path);
        fprintf(result_file, "The %2dst's page's passing paths is %u, centrality is %f, path is %s.\n", i + 1, out_num[highest_page_id], ((double)max_value) / all_path_num, page_list[highest_page_id].path);
        out_num[highest_page_id] = 0;
        max_value = 0;
    }
    fclose(result_file);

    return;
}