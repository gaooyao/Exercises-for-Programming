#include <stdio.h>
#include <stdlib.h>

#include "search_rawtrie.h"

TrieNode *root; //根节点

/*
* 函数名称：rawtrie_init_tree
* 函数功能：初始化树
*/
void rawtrie_init_tree()
{
    root = (TrieNode *)malloc(sizeof(TrieNode));

    root->count = 0;
    int i;
    for (i = 0; i < 256; i++)
    {
        root->child[i] = NULL;
    }
}

/*
* 函数名称：rawtrie_new_node
* 函数功能：创建新的节点
*/
TrieNode *rawtrie_new_node()
{
    TrieNode *obj = (TrieNode *)malloc(sizeof(TrieNode));
    obj->count = 0;
    int i;
    for (i = 0; i < 256; i++)
    {
        obj->child[i] = NULL;
    }
    return obj;
}

/*
* 函数名称：rawtrie_insert_recoder
* 函数功能：插入一条记录
*/
int rawtrie_insert_recoder(char *str)
{
    char *p = str;
    TrieNode *node = root;
    while (*p != '\0')
    {
        if (node->child[(unsigned char)*p] == NULL)
        {
            /* 对应的节点不存在，则创建一个节点 */
            node->child[(unsigned char)(*p)] = rawtrie_new_node();
        }
        node = node->child[(unsigned char)*p]; /* 指向子节点 */
        p++;
    }
    node->count++;
    return 1;
}

/*
* 函数名称：rawtrie_query_recoder
* 函数功能：查询记录是否存在
*/
int rawtrie_query_recoder(char *str)
{
    TrieNode *node = root; //指针指向root
    char *p = str;

    while (*p && node != NULL)
    {
        node = node->child[(unsigned char)*p]; /* 循环在树中查找所有节点 */
        p++;
    }

    if (node == NULL)
    {             //str未结束，而node已经不存在，则返回未找到
        return 0; /* 未找到 */
    }
    else
    {
        return (node->count > 0 ? (1) : (0)); //判断此节点是否为结束节点，若是则说明此纪录出现过，返回true
    }
}

/*
* 函数名称：rawtrie_destroy_tree
* 函数功能：释放树
*/
void rawtrie_destroy_tree()
{
    if (root != NULL)
    {
        free(root);
    }
}
