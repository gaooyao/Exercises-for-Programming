#include "search_mtrie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static MTNode *root;

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
unsigned int calc(unsigned int k, char *str)
{
    int n = k * M / 8;
    int l = k * M % 8;
    return (unsigned char)(((unsigned char)(str[n])) << l) >> (((8 / M) - 1) * M);
}
void free_node(MTNode *node)
{

    int i;
    for (i = 0; i < (1 << M); i++)
    {
        if (node->child[i])
        {
            free_node(node->child[i]);
        }
    }
    free(node);
}
void mtrie_init_tree()
{
    root = create_new_node();
}

int mtrie_insert_recoder(char *str)
{
    int num = strlen(str) * 8 / M;
    int i;
    MTNode *point = root;
    for (i = 0; i < num; i++)
    {
        unsigned int p = calc(i, str);
        if (!point->child[p])
        {
            point->child[p] = create_new_node();
        }
        point = point->child[p];
    }
    point->is_end = 1;
    return 1;
}

int mtrie_query_recoder(char *str)
{
    int num = strlen(str) * 8 / M;
    int i;
    MTNode *point = root;
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

void mtrie_destroy_tree()
{
    free_node(root);
}
