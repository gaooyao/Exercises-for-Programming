#include <stdio.h>
#include <stdlib.h>
#include "search_rawtrie.h"


TrieNode *root;//root
void rawtrie_init_tree()
{
    root = (TrieNode *)malloc(sizeof(TrieNode));
  
    root->count = 0;
    int i;
    for (i = 0; i < 256;i++)
    {
        root->child[i] = NULL;
    }
}
TrieNode* rawtrie_new_node(){
    TrieNode* obj = (TrieNode*)malloc(sizeof(TrieNode));
    obj->count = 0;
    int i;
    for (i = 0; i < 256;i++)
    {
        obj->child[i] = NULL;
    }
    return obj;
}

int rawtrie_insert_recoder(char *str){
    char* p = str;
    TrieNode* node = root;
    while (*p != '\0') {
        if (node->child[(unsigned char)*p] == NULL) {
            /* 对应的节点不存在，则申请一个节点 */
            node->child[(unsigned char)(*p)] = rawtrie_new_node();
        }
        node = node->child[(unsigned char)*p]; /* 指向子节点 */
        p++;
    }
    node->count++;
    return 1;
}

int rawtrie_query_recoder(char *str)
{
    TrieNode *node = root;//指针指向root
    char *p = str;

    while (*p && node != NULL) {
        node = node->child[(unsigned char)*p]; /* 循环在树中查找所有节点 */
        p++;
    }

    if (node == NULL) {//str未结束，而node已经不存在，则返回未找到
        return 0; /* 未找到 */
    } 
    else {
        return (node->count > 0 ? (1) : (0)); /*  */
        printf("\n");
    }
}
void rawtrie_destroy_tree()
{
    if (root != NULL) {
        free(root);
    }
    printf("destroy w\n");
}
