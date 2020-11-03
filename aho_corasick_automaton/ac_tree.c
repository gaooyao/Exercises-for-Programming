#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ac_tree.h"

#define MAX_QUEUE_LEN 512 //最大树深

static Node *root;     //树根
static Node *ac_state; //当前状态

Node *queue_start[MAX_QUEUE_LEN]; //此数组记录节点层数队列开始
Node *queue_end[MAX_QUEUE_LEN];   //此数组记录节点层数队列结束

/*
* 函数名称：create_new_node
* 函数功能：创建新的空节点
* 返回值：  Node指针
*/
Node *create_new_node()
{
    Node *_node = (Node *)malloc(sizeof(Node));
    _node->str = 0;
    _node->is_end = 0;
    _node->father = NULL;
    _node->child = NULL;
    _node->brother = NULL;
    _node->turn = NULL;
    _node->out = NULL;
    return _node;
}

/*
* 函数名称：search_child
* 函数功能：根据给定字符寻找子节点内是否有相同字符串的节点
* 参数：    node：在此节点的子节点内寻找
*           _ch:要寻找的字符
* 返回值：  Node指针
*/
Node *search_child(Node *node, char _ch)
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
void init_tree()
{
    root = create_new_node();
    ac_state = root;
    //初始化节点层数队列
    memset(queue_start, 0, sizeof(Node *) * MAX_QUEUE_LEN);
    memset(queue_end, 0, sizeof(Node *) * MAX_QUEUE_LEN);
    return;
}

//输出
char out[512];
char *ch = NULL;
void calc_str(Node *node)
{
    int k = 0;
    for (; k < 512; k++)
    {
        out[k] = 0;
    }
    k = 510;
    while (node != root)
    {
        out[k] = node->str;
        k--;
        node = node->father;
    }
    ch = &(out[k + 1]);
}

void add_num(Node *node, int line_num)
{
    if (!node->out)
    {
        node->out = (Out *)malloc(sizeof(Out));
        calc_str(node);
        node->out->str = (char *)malloc(strlen(ch) + 1);
        strcpy(node->out->str, ch);
        node->out->num = 1;
        node->out->line_1 = line_num;
        node->out->line_2 = 0;
        node->out->line_3 = 0;
        return;
    }
    else
    {
        node->out->num++;
        if (!node->out->line_2)
        {
            node->out->line_2 = line_num;
            return;
        }
        else
        {
            if (!node->out->line_3)
            {
                node->out->line_3 = line_num;
                return;
            }
        }
    }
}

Node *out_start;
Node *out_end;

void get_out(Node *node)
{
    if (node->out)
    {
        if (!out_start)
        {
            out_start = node;
            out_end = node;
        }
        else
        {
            out_end->turn = node;
            out_end = node;
        }
    }
    if (node->brother)
    {
        get_out(node->brother);
    }
    if (node->child)
    {
        get_out(node->child);
    }
}

void out_to_result()
{
    out_start = NULL;
    out_end = NULL;
    get_out(root);
    out_end->turn = NULL;
    Node *temp = out_start;
    FILE *file;
    file = fopen("result.txt", "w");
    while (temp)
    {
        fprintf(file, "%s %d %d %d %d\n", temp->out->str, temp->out->num, temp->out->line_1, temp->out->line_2, temp->out->line_3);
        temp = temp->turn;
    }
    fclose(file);
    return;
}

/*
* 函数名称：insert_recoder
* 函数功能：树中插入记录
* 参数：    str：被插入的字符串
*/
void insert_recoder(char *str)
{
    int i;
    Node *point = root;
    int len = strlen(str);
    for (i = 0; i < len; i++)
    {
        int j = 1; //标志是否需要创建新节点,1为需要
        Node *point_father = point;
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
            Node *new_node = create_new_node();

            new_node->str = str[i];
            new_node->father = point_father;
            //新节点加入节点层数队列
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
    point->is_end = 1;
}

void make_turn()
{
    int i = 0;
    for (; i < MAX_QUEUE_LEN; i++)
    {
        while (queue_start[i])
        {
            Node *now_node = queue_start[i];
            queue_start[i] = queue_start[i]->turn;

            now_node->turn = root;
            Node *turn_node = now_node->father->turn;
            while (turn_node)
            {
                Node *_node = turn_node;
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

void query_recoder(char *str, int line_num)
{
    int i;
    Node *child;
    short len = strlen(str);
    ac_state = root;
    for (i = 0; i < len; i++)
    {
        child = search_child(ac_state, str[i]);
        if (child)
        {
            ac_state = child;
            Node *out_node = ac_state;
            while (out_node)
            {
                if (out_node->is_end)
                {
                    add_num(out_node, line_num);
                }
                out_node = out_node->turn;
            }
        }
        else
        {
            if (ac_state->turn)
            {
                ac_state = ac_state->turn;
                i--;
            }
        }
    }
}
