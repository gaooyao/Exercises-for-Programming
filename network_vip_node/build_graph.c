#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "build_graph.h"
#include "string_file_reader.h"

char base_path[4096] = {0};
int base_path_len = 0;
unsigned int file_num = 0;

FileNode **file_index_list;

FileNode *file_queue_front;
FileNode *file_queue_tail;

void create_file_queue(char *file_name, char *file_path)
{
    int file_name_len = strlen(file_name);
    if (file_name[file_name_len - 2] == 'm' && file_name[file_name_len - 1] == 'l')
    {
        file_num++;
        //printf("%s\n", file_name);
        char *ch = (char *)malloc(strlen(file_path) + strlen(file_name) - base_path_len + 2);
        strcpy(ch, &file_path[base_path_len]);
        strcat(ch, dir_sp);
        strcat(ch, file_name);
        FileNode *file_node = (FileNode *)malloc(sizeof(FileNode));
        file_node->path = ch;
        file_node->next = NULL;
        file_queue_tail->next = file_node;
        file_queue_tail = file_node;
    }
}

void travel_file(char *path, void (*operator)(char *file_name, char *file_path))
{
    DIR *dir;
    struct dirent *ptr;
    char dir_path[4096];
    //printf("%s\n", path);
    if ((dir = opendir(path)) == NULL)
    {
        return;
    }
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
        {
            continue;
        }
        else
        {
            int file_name_len = strlen(ptr->d_name);
            if (ptr->d_type == 8)
            {
                operator(ptr->d_name, path);
            }
            else if (ptr->d_type == 4)
            {
                memset(dir_path, '\0', sizeof(dir_path));
                strcpy(dir_path, path);
                strcat(dir_path, dir_sp);
                strcat(dir_path, ptr->d_name);
                travel_file(dir_path, create_file_queue);
            }
        }
    }
    closedir(dir);
}

#define MAX_QUEUE_LEN 512 //最大树深，这里假设最大512

static AcTreeNode *root;     //树根
static AcTreeNode *ac_state; //ac机当前状态

AcTreeNode *queue_start[MAX_QUEUE_LEN]; //此数组记录节点层数队列开始
AcTreeNode *queue_end[MAX_QUEUE_LEN];   //此数组记录节点层数队列结束

/*
* 函数名称：create_new_node
* 函数功能：创建新的空节点
* 返回值：  Node指针
*/
AcTreeNode *create_new_node()
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
* 函数名称：search_child
* 函数功能：根据给定字符寻找子节点内是否有相同字符串的节点
* 参数：    node：在此节点的子节点内寻找
*           _ch:要寻找的字符
* 返回值：  Node指针
*/
AcTreeNode *search_child(AcTreeNode *node, char _ch)
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
* 函数名称：init_tree
* 函数功能：树的初始化
*/
void init_ac_tree()
{
    root = create_new_node();
    ac_state = root;
    //初始化节点层数队列
    memset(queue_start, 0, sizeof(AcTreeNode *) * MAX_QUEUE_LEN);
    memset(queue_end, 0, sizeof(AcTreeNode *) * MAX_QUEUE_LEN);
    return;
}

/*
* 函数名称：insert_recoder
* 函数功能：树中插入记录
* 参数：    str：被插入的字符串
*/
void insert_ac_recoder(char *str, int id)
{
    int i;
    AcTreeNode *point = root;
    int len = strlen(str);
    for (i = 0; i < len; i++)
    {
        int j = 1; //标志是否需要创建新节点,1为需要
        AcTreeNode *point_father = point;
        point = point->child;
        while (point) //遍历子节点，寻找bit是否已出现
        {
            if (point->str == str[i])
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
            AcTreeNode *new_node = create_new_node();

            new_node->str = str[i];
            new_node->father = point_father;
            new_node->file_id = id;
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
* 函数名称：make_turn
* 函数功能：遍历每层的节点，构建它们的失效指针
*/
void make_turn()
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
                _node = search_child(_node, now_node->str);
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
* 函数名称：query_recoder
* 函数功能：查询函数，按字节遍历所给字符串，在不同状态间跳转，并输出
*/
void query_recoder(char *str, int file_node_id)
{
    int i;
    AcTreeNode *child;
    AcTreeNode *out_node;
    short len = strlen(str);
    ac_state = root;
    for (i = 0; i < len; i++) //按字节遍历字符串
    {
        child = search_child(ac_state, str[i]); //寻找能匹配的子节点
        if (child)
        {
            //若找到则判断是否有输出，然后继续遍历下个字节
            ac_state = child;
            out_node = ac_state;
            while (out_node)
            {
                if (out_node->is_end)
                {
                    //printf("file %s has link to %s.\n", file_index_list[file_node_id]->path, file_index_list[out_node->file_id]->path);
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
                i--;
            }
        }
    }
}

void build_ac_tree()
{
    FileNode *file;
    file = file_queue_front->next;
    file_index_list = (FileNode **)malloc(sizeof(FileNode *) * file_num);
    int i = 0;
    while (file)
    {
        file_index_list[i] = file;
        insert_ac_recoder(file->path, i);
        file = file->next;
        i++;
        //printf("%d\n", i);
    }
}

void match_link()
{
    char *str = NULL;
    FileHandler *file;
    FileNode *file_node;
    char file_path[4096];
    file_node = file_queue_front->next;
    int i = 0;
    while (file_node)
    {
        strcpy(file_path, base_path);
        strcat(file_path, file_node->path);
        file = open_file(file_path, "rb");
        while (read_line(file, &str))
        {
            query_recoder(str, i);
        }
        close_file(file);
        file_node = file_node->next;
        i++;
        //printf("%d\n", i);
    }
}

void build_graph()
{
    file_queue_front = (FileNode *)malloc(sizeof(FileNode));
    file_queue_front->path = 0;
    file_queue_front->next = NULL;
    file_queue_tail = file_queue_front;
    getcwd(base_path, 4096);
    strcat(base_path, web_page_dir_path);
    base_path_len = strlen(base_path);
    travel_file(base_path, create_file_queue);
    init_ac_tree();
    build_ac_tree();
    make_turn();
    match_link();
}
