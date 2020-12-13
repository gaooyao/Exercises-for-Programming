
//网页结点
typedef struct page
{
    int link_num;   //连接的网页数目
    char *path;     //网页路径
    int *link_list; //连接的网页列表
} Page;

//路径结构体
typedef struct path
{
    unsigned int node_num; //连接的网页数目
    int *node_list;        //连接的网页列表
    struct path *next;     //下一条路径信息
} Path;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

int int_len = sizeof(int);
int char_len = sizeof(char);
int page_number = 0;            //网页数量
unsigned long all_path_num = 0; //所有路径数目
Page *page_list;                //存储网页结点信息

void brandes_between()
{
    printf("Start read graph.bin.\n");
    clock_t start_time, end_time;
    start_time = clock();
    FILE *graph_bin;
    //打开graph.bin
    if ((graph_bin = fopen("graph.bin", "rb")) == NULL)
    {
        printf("Open graph.bin fail!\n");
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
    fclose(graph_bin);
    unsigned int *out_num = (int *)malloc(int_len * page_number); //存储每个网页经过它的最短路径数
    memset(out_num, 0, int_len * page_number);
    int *is_visited = (int *)malloc(int_len * page_number);         //存储是否已经计算过i节点到一个节点的最短路径
    int *new_visited = (int *)malloc(int_len * page_number * 1024); //缓存新访问的节点
    for (i = 0; i < page_number; i++)
    {
        printf("Calc node %d.\n", i);
        memset(is_visited, 0, int_len * page_number); //所有节点都没被访问过
        is_visited[i] = 1;
        memset(new_visited, 0, int_len * page_number);   //没有新访问的节点
        Path *path_point = (Path *)malloc(sizeof(Path)); //创建路径信息队列
        path_point->node_num = 1;
        path_point->node_list = (int *)malloc(int_len);
        path_point->node_list[0] = i;
        path_point->next = NULL;
        Path *path_queue_tail = path_point; //路径信息队列的队尾
        int old_path_num = 0;               //路径信息队列中旧路径的数目
        int new_path_num = 1;               //路径信息队列中新路径的数目
        /*
            每轮遍历访问所有旧路经信息，找到每条老路径末尾节点能到达的新节点并以此创建新路径，
            直到某一轮遍历找不到任何新路径，则i节点能够到达的所有节点寻找完毕
        */
        do
        {
            old_path_num = new_path_num;
            new_path_num = 0;
            while (old_path_num) //遍历所有旧路经
            {
                Page *tail_page = &page_list[path_point->node_list[path_point->node_num - 1]]; //取该路径上最后一个网页节点
                int is_create_new_path = 0;                                                    //记录是否基于该路径创建了新的路径
                //遍历该网页节点的出链信息，若一个出链指向的节点未被访问过，则基于当前路径创建一条新路径
                for (j = 0; j < tail_page->link_num; j++)
                {
                    if (!is_visited[tail_page->link_list[j]]) //判断出链指向的节点是否被访问过
                    {
                        is_create_new_path = 1;                        //表示添加了新的路径
                        Path *new_path = (Path *)malloc(sizeof(Path)); //未被访问，创建新路径
                        new_path->node_num = path_point->node_num + 1;
                        new_path->next = NULL;
                        new_path->node_list = (int *)malloc(int_len * new_path->node_num);
                        //创建新路径的路径信息为老路径信息加新访问的节点
                        memcpy(new_path->node_list, path_point->node_list, path_point->node_num * int_len);
                        new_path->node_list[new_path->node_num - 1] = tail_page->link_list[j];
                        //创建的新路径加入路径信息队列
                        path_queue_tail->next = new_path;
                        path_queue_tail = new_path;
                        if (new_path->node_num > 2)
                        {
                            all_path_num++; //总路径数加一
                        }
                        //新被访问的节点加入新访问节点队列
                        new_visited[new_path_num] = tail_page->link_list[j];
                        new_path_num++;
                    }
                }
                //若没有创建新路径，则统计该路径下所有经过的节点，增加经过次数
                if (!is_create_new_path)
                {
                    //路径上从第二个节点到倒数第二个节点，被经过次数依次减少，第二个节点被经过次数为此路径总节点数-2
                    for (k = 1; k < path_point->node_num - 1; k++)
                    {
                        out_num[path_point->node_list[k]] = (out_num[path_point->node_list[k]] + (path_point->node_num - k - 1));
                    }
                }
                //释放老路径
                Path *temp_path = path_point;
                path_point = path_point->next;
                free(temp_path->node_list);
                free(temp_path);
                old_path_num--;
            }
            //遍历完老路径之后，把所有新被访问到的节点放入已被访问节点列表
            for (j = 0; j < new_path_num; j++)
            {
                is_visited[new_visited[j]] = 1;
            }
        } while (new_path_num);
    }
    //输出被经过次数最多的前20个网页
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

#include <stdio.h>
#include <unistd.h>

int main()
{
    clock_t s, e;
    s = clock();
    brandes_between(); //计算介数
    e = clock();
    printf("Finished, all time: %.3f s.\n", (float)(e - s) / CLOCKS_PER_SEC);
    sleep(3);
    getchar();
    return 0;
}