#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "search_mtrie.h"

static MTNode *root; //M叉树的树根

/*
* 函数名称：create_new_node
* 函数功能：创建新的节点
*/
MTNode *create_new_node()
{
    int i;
    MTNode *node = (MTNode *)malloc(sizeof(MTNode));
    node->is_end = 0;
    for (i = 0; i < (1 << M); i++)
    {
        node->child[i] = NULL;
    }
    return node;
}

/*
* 函数名称：calc
* 函数功能：根据所给字符串，位置k以及叉数M，计算字符串K位置应在第几分叉
*/
unsigned int calc(unsigned int k, char *str)
{
    int n = k * M / 8;
    int l = k * M % 8;
    return (unsigned char)(((unsigned char)(str[n])) << l) >> (((8 / M) - 1) * M);
}

/*
* 函数名称：free_node
* 函数功能：释放一个节点
*/
void free_node(MTNode *node)
{
    int i = (1 << M) - 1;
    for (; i >= 0; i--)
    {
        if (node->child[i])
        {
            free_node(node->child[i]);
        }
    }
    free(node);
}

/*
* 函数名称：mtrie_init_tree
* 函数功能：初始化树
*/
void mtrie_init_tree()
{
    root = create_new_node();
}

/*
* 函数名称：mtrie_insert_recoder
* 函数功能：树中插入一个节点
*/
int mtrie_insert_recoder(char *str)
{
    int num = strlen(str) * 8 / M; //计算该字符串在树中的深度
    int i;
    MTNode *point = root;
    //从根节点开始，依次判断该字符串在树中路径上的节点是否存在，存在则继续向下遍历，不存在则创建新的节点
    for (i = 0; i < num; i++)
    {
        unsigned int p = calc(i, str);
        if (!point->child[p]) //判断对应子节点是否存在
        {
            point->child[p] = create_new_node(); //不存在则创建新节点
        }
        point = point->child[p];
    }
    point->is_end = 1; //最后一个节点的is_end置true表明走过此路径的字符串出现过
    return 1;
}

/*
* 函数名称：mtrie_query_recoder
* 函数功能：查询字符串是否出现过
*/
int mtrie_query_recoder(char *str)
{
    int num = strlen(str) * 8 / M;
    int i;
    MTNode *point = root;
    //从根节点开始，依次判断该字符串在树中路径上的节点是否存在，存在则继续向下遍历，不存在则返回false
    for (i = 0; i < num; i++)
    {
        unsigned int p = calc(i, str);
        if (!point->child[p])
        {
            return 0;
        }
        point = point->child[p];
    }
    return num ? point->is_end ? 1 : 0 : 0;
}

/*
* 函数名称：mtrie_destroy_tree
* 函数功能：释放树
*/
void mtrie_destroy_tree()
{
    free_node(root);
}
