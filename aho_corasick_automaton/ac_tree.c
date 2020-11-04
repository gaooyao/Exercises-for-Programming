#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ac_tree.h"

#define MAX_QUEUE_LEN 512 //最大树深，这里假设最大512

static Node *root;     //树根
static Node *ac_state; //ac机当前状态

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

char out[512];//暂存从node到root拼出来的字符串
char *ch = NULL;//指向拼出来的字符串的开始位置

/*
* 函数名称：calc_str
* 函数功能：把node到root的路径上的字符拼成字符串并返回
*/
void calc_str(Node *node)
{
    //初始化数组
    int k = 0;
    for (; k < 512; k++)
    {
        out[k] = 0;
    }
    k = 510;
    while (node != root) //从当前节点开始向上找，直到root
    {
        out[k] = node->str; //放入out数组（倒着放，因为是倒着遍历）
        k--;
        node = node->father;
    }
    ch = &(out[k + 1]); //ch指针指向拼出来的字符串的首字符
}

/*
* 函数名称：add_num
* 函数功能：给node对应的字符串的出现次数加一
*/
void add_num(Node *node, int line_num)
{
    if (!node->out) //如果此node还没有绑定Out结构体，则创建并绑定
    {
        node->out = (Out *)malloc(sizeof(Out));
        calc_str(node); //拼出字符串
        //把拼出来的字符串放入Out的str里面
        node->out->str = (char *)malloc(strlen(ch) + 1);
        strcpy(node->out->str, ch);
        node->out->num = 1; //出现次数
        node->out->line_1 = line_num;
        node->out->line_2 = 0;
        node->out->line_3 = 0;
        return;
    }
    else
    //执行到这里说明不是第一次出现，只需num+1，更新行号
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

/*
* 函数名称：quick_sort
* 函数功能：快速排序算法，具体参考数据结构课本
* 参数说明：arr:存储所有有输出节点的数组
*/
void quick_sort(Node **arr, int begin, int end)
{
    if (begin < end)
    {
        Node *temp = arr[begin];
        int i = begin;
        int j = end;
        while (i < j)
        {
            while (i < j && arr[j]->out->num <= temp->out->num)
            {
                j--;
            }
            arr[i] = arr[j];
            while (i < j && arr[i]->out->num > temp->out->num)
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
* 函数名称：get_out
* 函数功能：一个递归算法，把所有输出的节点用它们的turn指针连起成一个队列
*/
Node *out_start; //队头
Node *out_end;   //队尾
int out_num = 0; //记录有多少个输出节点
void get_out(Node *node)
{
    if (node->out) //如果有out，则把此节点加入队列中
    {
        out_num++;      //输出节点数量加一
        if (!out_start) //如果out_start，out_start为空则都指向此节点（队列的第一个节点）
        {
            out_start = node;
            out_end = node;
        }
        else
        {
            //入队
            out_end->turn = node;
            out_end = node;
        }
    }
    //遍历此节点的兄弟节点和子节点，先兄弟后子（广度优先）
    if (node->brother)
    {
        get_out(node->brother);
    }
    if (node->child)
    {
        get_out(node->child);
    }
}

/*
* 函数名称：out_to_result
* 函数功能：此函数被main调用，负责遍历树，找到输出节点，整理并输出
*/
void out_to_result()
{
    //第一步，把输出节点穿成串
    out_start = NULL;
    out_end = NULL;
    get_out(root);
    out_end->turn = NULL;
    //第二步，把这些节点放到一个数组里，为快速排序做准备
    Node *node = out_start;
    Node *out_node_list[out_num];
    int i = 0;
    while (node)
    {
        out_node_list[i] = node;
        node = node->turn;
        i++;
    }
    //第三步，快速排序
    quick_sort(out_node_list, 0, out_num - 1);
    //第四步，此时已排好序，按序输出到文件
    FILE *file;
    Out *out;
    file = fopen("result.txt", "w");
    for (i = 0; i < out_num; i++)
    {
        out = out_node_list[i]->out;
        fprintf(file, "%s %d %d %d %d\n", out->str, out->num, out->line_1, out->line_2, out->line_3);
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
            Node *now_node = queue_start[i]; //now_node指向当前被构建失效指针的节点
            queue_start[i] = queue_start[i]->turn;
            now_node->turn = root; //失效指针先默认指向root
            Node *turn_node = now_node->father->turn;
            while (turn_node) //寻找失效指针应该指向的节点
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

/*
* 函数名称：query_recoder
* 函数功能：查询函数，按字节遍历所给字符串，在不同状态间跳转，并输出
*/
void query_recoder(char *str, int line_num)
{
    int i;
    Node *child;
    Node *out_node;
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
                    add_num(out_node, line_num);
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
