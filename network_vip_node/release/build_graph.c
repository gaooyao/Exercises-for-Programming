#define web_page_dir_path "/news.sohu.com" //网页文件存放根目录

#define link_mask_num 7 //网站前缀掩码

#define dir_sp "/" //文件夹分隔符格式

#define char_len sizeof(char)

#define int_len sizeof(int)

typedef struct file_node //代表网页文件的数据结点
{
    char *path; //此网页的路径
    int id;
    struct file_node *next; //队列中下一个结点
} FileNode;

//状态机的节点，本次实验借助构建ac状态机搜索出链信息
typedef struct ac_tree_node
{
    char str;
    unsigned short is_end;        //此节点是否对应某字符串结束
    struct ac_tree_node *father;  //父节点
    struct ac_tree_node *child;   //子节点
    struct ac_tree_node *brother; //兄弟节点
    struct ac_tree_node *turn;    //失效节点
    int file_id;                  //此节点对应的FileNode在file_index_list数组中的位置
} AcTreeNode;

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

char base_path[4096] = {0}; //此数组储存存放网页的文件夹的路径
int base_path_len = 0;      //存放网页的文件夹的路径的长度
unsigned int file_num = 0;  //网页文件数量

FileNode *file_queue_front; //网页文件节点队列的队头
FileNode *file_queue_tail;  //网页文件节点队列的队尾

/*
函数功能：根据传入的文件名，文件路径创建对应的FileNode结点，并放入文件结点队列中
参数说明：
         file_name：文件名
         file_path：文件路径
*/
void create_file_queue(char *file_name, char *file_path)
{
    int file_name_len = strlen(file_name);
    if (file_name[file_name_len - 2] == 'm' && file_name[file_name_len - 1] == 'l') //根据文件名最后两个字母判断文件是否是网页文件
    {
        //创建字符串文件位置信息字符串
        char *ch = (char *)malloc(strlen(file_path) + strlen(file_name) - base_path_len + 2);
        strcpy(ch, &file_path[base_path_len]);
        strcat(ch, dir_sp);
        strcat(ch, file_name);
        //创建FileNode结点并赋初值
        FileNode *file_node = (FileNode *)malloc(sizeof(FileNode));
        file_node->path = ch;
        file_node->next = NULL;
        file_node->id = file_num;
        //创建的FileNode加入全局文件结点队列
        file_queue_tail->next = file_node;
        file_queue_tail = file_node;
        file_num++; //网页文件数量加一
    }
}

/*
函数功能：根据传入路径信息，遍历此路径下所有文件并执行相关操作
参数说明：
         path：被遍历的文件夹路径
         operator：一个函数指针，遍历找到文件时调用此函数进行相关处理
*/
void travel_file(char *path, void (*operator)(char *file_name, char *file_path))
{
    DIR *dir;
    struct dirent *ptr;
    char dir_path[4096];               //用来存储临时文件夹路径信息
    if ((dir = opendir(path)) == NULL) //打开文件夹失败
    {
        return;
    }
    while ((ptr = readdir(dir)) != NULL) //读取文件夹内容，进行相关操作
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) //跳过.和..文件夹
        {
            continue;
        }
        else
        {
            if (ptr->d_type == 8)
            {
                operator(ptr->d_name, path); //如果是文件则进行处理
            }
            else if (ptr->d_type == 4) //如果是文件夹则继续进行遍历
            {
                //拼接出子文件夹路径
                memset(dir_path, '\0', sizeof(dir_path));
                strcpy(dir_path, path);
                strcat(dir_path, dir_sp);
                strcat(dir_path, ptr->d_name);
                //遍历子文件夹
                travel_file(dir_path, operator);
            }
        }
    }
    closedir(dir);
}

#define MAX_QUEUE_LEN 512               //ac状态机最大树深，这里假设最大512
static AcTreeNode *root;                //ac状态机树根
static AcTreeNode *ac_state;            //ac机当前状态
AcTreeNode *queue_start[MAX_QUEUE_LEN]; //此数组记录ac状态机节点层数队列开始
AcTreeNode *queue_end[MAX_QUEUE_LEN];   //此数组记录ac状态机节点层数队列结束

/*
函数功能：创建新的AcTreeNode
返回值：  AcTreeNode指针
*/
AcTreeNode *ac_create_newnode()
{
    AcTreeNode *_node = (AcTreeNode *)malloc(sizeof(AcTreeNode));
    _node->str = 0;
    _node->is_end = 0;
    _node->father = NULL;
    _node->child = NULL;
    _node->brother = NULL;
    _node->turn = NULL;
    _node->file_id = 0;
    return _node;
}

/*
函数名称：ac_search_child
函数功能：根据给定字符寻找子节点内是否有相同字符串的节点
参数说明：
         node：在此节点的子节点内寻找
         _ch:要寻找的字符
*/
AcTreeNode *ac_search_child(AcTreeNode *node, char _ch)
{
    node = node->child;
    while (node) //遍历子节点
    {
        if (node->str == _ch)
        {
            return node;
        }
        node = node->brother;
    }
    return NULL;
}

/*
* 函数功能：ac状态机树的初始化
*/
void ac_init_tree()
{
    root = ac_create_newnode(); //创建树根
    ac_state = root;
    //初始化节点层数队列
    memset(queue_start, 0, sizeof(AcTreeNode *) * MAX_QUEUE_LEN);
    memset(queue_end, 0, sizeof(AcTreeNode *) * MAX_QUEUE_LEN);
    return;
}

/*
函数功能：ac状态机树中插入一条记录
参数说明：
         file：被插入的文件结点
         id： 此记录对应的FileNode结点在file_index_list数组中的位置，可通过此id找到对应的FileNode结点
*/
void ac_insert_recoder(FileNode *file)
{
    int i;
    AcTreeNode *point = root;
    int len = strlen(file->path);
    for (i = 0; i < len; i++)
    {
        int j = 1; //标志是否需要创建新节点,1为需要
        AcTreeNode *point_father = point;
        point = point->child;
        while (point) //遍历子节点，寻找bit是否已出现
        {
            if (point->str == file->path[i])
            {
                //找到可继续遍历的节点
                j = 0;
                break;
            }
            //未找到则遍历下一个brother
            if (!point->brother)
            {
                break;
            }
            point = point->brother;
        }
        if (j)
        {
            //需要创建新节点
            AcTreeNode *new_node = ac_create_newnode();
            new_node->str = file->path[i];
            new_node->father = point_father;
            new_node->file_id = file->id;
            //把每一层的节点连成队列，新节点加入节点层数队列
            if (!queue_start[i])
            {
                queue_start[i] = new_node;
                queue_end[i] = new_node;
            }
            else
            {
                queue_end[i]->turn = new_node;
                queue_end[i] = new_node;
            }
            //修改与新节点有关的指针
            if (point)
            {
                point->brother = new_node;
            }
            else
            {
                point_father->child = new_node;
            }
            point = new_node;
        }
    }
    point->is_end = 1; //最后一个节点打个结束标记
}

/*
函数功能：遍历ac状态机每层的节点，构建它们的失效指针
*/
void ac_make_turn()
{
    int i = 0;
    for (; i < MAX_QUEUE_LEN; i++) //遍历每一层的节点队列，构建他们的失效指针
    {
        while (queue_start[i])
        {
            AcTreeNode *now_node = queue_start[i]; //now_node指向当前被构建失效指针的节点
            queue_start[i] = queue_start[i]->turn;
            now_node->turn = root; //失效指针先默认指向root
            AcTreeNode *turn_node = now_node->father->turn;
            while (turn_node) //寻找失效指针应该指向的节点
            {
                AcTreeNode *_node = turn_node;
                _node = ac_search_child(_node, now_node->str);
                if (_node)
                {
                    now_node->turn = _node;
                    break;
                }
                else
                {
                    turn_node = turn_node->turn;
                }
            }
        }
    }
}

/*
函数说明：根据travel_file函数遍历出的file_queue队列，找到每个文件的路径信息作为模板字符串插入ac状态机中
*/
void build_ac_tree()
{
    FileNode *file;
    file = file_queue_front->next;
    while (file)
    {
        /*
        FileNode里的文件路径信息作为模板字符串插入ac状态机，并传入此FileNode在file_index_list中的位置信息，
        ac状态机创建的结点存有此id，通过此id就能在file_index_list中找到对应的FileNode结点。
        为什么用id表示一个FileNode结点，不直接用指针？
        给每个网页文件的FileNode绑定一个id，这样出链信息中存放一个id即可。保存到文件后此id依然方便有效，但指针就会失效。
        */
        ac_insert_recoder(file); //路径作为字符串插入状态机
        file = file->next;
    }
}

/*
函数说明：遍历FileNode队列，根据FileNode中存放的文件路径打开相应文件，并把文件作为字符串在构建好的ac状态机中查询，查询函数ac_query_recoder负责处理输出信息。
*/
void match_link()
{
    char ch = 0;
    FILE *graph_file;
    FILE *page_file;

    int *out_link_list = (int *)malloc(int_len * file_num);
    int out_link_num;

    //打开输出文件
    if ((graph_file = fopen("graph.bin", "wb")) == NULL)
    {
        printf("open data file fail!");
        return;
    }
    //写入文件总数目
    fwrite(&file_num, sizeof(int), 1, graph_file);
    int i = 0;
    AcTreeNode *child;
    AcTreeNode *out_node;
    FileNode *file_node;
    char file_path[4096];
    file_node = file_queue_front->next;
    while (file_node) //遍历FileNode队列
    {
        ac_state = root;
        out_link_num = 0;
        //拼接出文件完整路径
        strcpy(file_path, base_path);
        strcat(file_path, file_node->path);
        //打开输出文件
        if ((page_file = fopen(file_path, "rb")) == NULL)
        {
            printf("open page file fail!");
            return;
        }
        while (EOF != (ch = fgetc(page_file)))
        {
            child = ac_search_child(ac_state, ch); //寻找能匹配的子节点
            if (child)
            {
                //若找到则判断是否有输出，然后继续遍历下个字节
                ac_state = child;
                out_node = ac_state;
                while (out_node)
                {
                    if (out_node->is_end)
                    {
                        //若一个网页中有多个相同的出链信息，则只应算做一次，所以要先判断是否已经统计过此出链信息
                        int need_add_new_out_flag = 1;
                        int k;
                        for (k = 0; k < out_link_num; k++)
                        {
                            if (out_link_list[k] == out_node->file_id)
                            {
                                need_add_new_out_flag = 0;
                                break;
                            }
                        }
                        if (need_add_new_out_flag) //需要增加一条出链信息
                        {
                            out_link_list[out_link_num] = out_node->file_id;
                            out_link_num++;
                        }
                    }
                    out_node = out_node->turn;
                }
            }
            else
            {
                //若没找到则在失效指针指向的节点内继续寻找
                if (ac_state->turn)
                {
                    ac_state = ac_state->turn;
                }
            }
        }
        //写入文件路径
        fwrite(file_node->path, strlen(file_node->path), 1, graph_file);
        //写入一个字符串结束符
        char space = 0;
        fwrite(&space, sizeof(char), 1, graph_file);
        //写入文件出链数目
        fwrite(&out_link_num, int_len, 1, graph_file);
        //写入每条出链指向的id
        fwrite(out_link_list, int_len, out_link_num, graph_file);
        fclose(page_file);
        file_node = file_node->next;
        i++;
        if (!(i % 10000))
        {
            printf("Finished webpage number: %6d.\n", i);
        }
    }
    fclose(graph_file);
}

/*
函数说明：此函数调用各子函数，完成graph.bin构建任务
*/
void build_graph()
{
    printf("Start generate graph.bin.\n");
    clock_t start_time, end_time;
    start_time = clock();
    //创建FileNode队列
    file_queue_front = (FileNode *)malloc(sizeof(FileNode));
    file_queue_front->path = 0;
    file_queue_front->next = NULL;
    file_queue_tail = file_queue_front;
    //拼接存放网页的文件夹的绝对路径
    getcwd(base_path, 4096);
    strcat(base_path, web_page_dir_path);
    base_path_len = strlen(base_path);
    printf("The directory where the webpage stored is %s.\n", base_path);
    //遍历文件夹，构建FileNode队列
    printf("Start map all webpage...\n");
    travel_file(base_path, create_file_queue);
    printf("Map webpage finished, the webpage number is %d.\n", file_num);
    //初始化ac状态机
    printf("Start build ac-state-machine.\n");
    ac_init_tree();
    //文件信息插入ac状态机
    build_ac_tree();
    //构建ac状态机失效指针
    ac_make_turn();
    //读取每个网页文件并在ac状态机中匹配
    printf("Start matching in the ac-state-machine.\n");
    match_link();
    //输出所有网页出链信息到graph.bin
    printf("Start exporting all information to the graph.bin.\n");
    end_time = clock();
    printf("Generate graph.bin finished, use time %f seconds.\n", (float)(end_time - start_time) / CLOCKS_PER_SEC);
}

#include <stdio.h>
#include <unistd.h>

int main()
{
    build_graph(); //输出出链信息到graph.bin
    sleep(3);
    getchar();
    return 0;
}
