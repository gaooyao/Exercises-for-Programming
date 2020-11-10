#ifndef search_bplus
#define search_bplus

#include <stdint.h>

#define CHILD_NUM 64 //B+树的叉数

typedef struct bptree //B+树的节点
{
    uint64_t *keys;        //关键字key
    void **pointers;       //指向叶子节点内存储的字符串
    uint16_t is_leaf : 1;  //用来判断节点是否为叶子节点
    uint16_t nr_keys : 15; //节点中已存放了多少个关键字
} BPTree;

void bplus_init_tree();              //初始化树
int bplus_insert_recoder(char *str); //插入
int bplus_query_recoder(char *str);  //查询
void bplus_destroy_tree();           //释放树

#endif