/* reference:https://github.com/vedantk/bptree */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "murmurhash.h"
#include "search_bplus.h"

/*
* 函数名称：bpt_alloc
* 函数功能：创建bplus树节点并初始化
*/
static BPTree *bpt_alloc()
{
    BPTree *bpt = malloc(sizeof(BPTree));
    if (!bpt)
    {
        return NULL;
    }
    bpt->keys = malloc(sizeof(uint64_t) * (CHILD_NUM - 1));
    if (!bpt->keys)
    {
        goto release0;
    }
    bpt->pointers = calloc(CHILD_NUM, sizeof(void *));
    if (!bpt->pointers)
    {
        goto release1;
    }
    bpt->is_leaf = 1;
    bpt->nr_keys = 0;
    bpt->pointers[0] = NULL;
    return bpt;
release1:
    free(bpt->keys);
release0:
    free(bpt);
    abort();
    return NULL;
}

/*
* 函数名称：bptree_alloc
* 函数功能：创建bplus树节点并初始化
*/
BPTree *bptree_alloc(uint64_t key, void *val)
{
    BPTree *bpt = bpt_alloc();
    bpt->nr_keys = 1;
    bpt->keys[0] = key;
    bpt->pointers[1] = val;
    return bpt;
}

/*
* 函数名称：split
* 函数功能：返回X的一半
*/
static inline int split(int x)
{
    return (x >> 1) + (x & 1);
}

/*
* 函数名称：bpt_bisect
* 函数功能：找到给定key的索引
*/
static int bpt_bisect(BPTree *bpt, uint64_t key)
{
    int low = 0;
    int high = bpt->nr_keys - 2;
    int mid = low + ((high - low) / 2);
    while (!(bpt->keys[mid] <= key && key < bpt->keys[mid + 1]))
    {
        assert(low < high);
        assert(bpt->keys[mid] != bpt->keys[mid + 1]);
        if (bpt->keys[mid] < key)
        {
            low = mid;
        }
        else if (bpt->keys[mid] > key)
        {
            high = mid - 1;
        }
        else
        {
            return mid;
        }
        mid = low == mid ? (mid + 1) : low + ((high - low) / 2);
    }
    return mid;
}

/*
* 函数名称：bpt_index
* 函数功能：查找key的指针索引
*/
static void bpt_index(BPTree *bpt, uint64_t key, int *k, int *p)
{
    assert(bpt->nr_keys > 0);
    const int last_idx = bpt->nr_keys - 1;
    if (key < bpt->keys[0])
    {
        *k = *p = 0;
    }
    else if (key >= bpt->keys[last_idx])
    {
        *k = last_idx;
        *p = last_idx + 1;
    }
    else
    {
        *k = bpt_bisect(bpt, key);
        *p = *k + 1;
    }
}

/*
* 函数名称：bptree_search
* 函数功能：查找key应在的叶子
*/
BPTree *bptree_search(BPTree *bpt, uint64_t key)
{
    int kidx, pidx;
    while (!bpt->is_leaf)
    {
        bpt_index(bpt, key, &kidx, &pidx);
        bpt = bpt->pointers[pidx];
        assert(bpt);
        assert(bpt->nr_keys >= split(CHILD_NUM) - 1);
    }
    return bpt;
}

/*
* 函数名称：bptree_exists
* 函数功能：查找key是否存在
*/
BPTree *bptree_exists(BPTree *bpt, uint64_t key)
{
    int kidx, pidx;
    bpt = bptree_search(bpt, key);
    bpt_index(bpt, key, &kidx, &pidx);
    return (bpt && bpt->keys[kidx] == key) ? bpt : NULL;
}

/*
* 函数名称：bpt_inject
* 函数功能：树中插入节点
*/
static void bpt_inject(BPTree *bpt, int pidx, uint64_t key, void *val)
{
    assert(bpt->nr_keys < CHILD_NUM - 1);
    for (int i = bpt->nr_keys; i > pidx; --i)
    {
        bpt->pointers[i + 1] = bpt->pointers[i];
    }
    bpt->pointers[pidx + 1] = val;
    for (int i = bpt->nr_keys - 1; i >= pidx; --i)
    {
        bpt->keys[i + 1] = bpt->keys[i];
    }
    bpt->keys[pidx] = key;
    ++bpt->nr_keys;
    BPTree *pred = bpt->pointers[pidx];
    if (!bpt->is_leaf && pred && pred->is_leaf)
    {
        BPTree *succ = bpt->pointers[pidx + 1];
        succ->pointers[0] = pred->pointers[0];
        pred->pointers[0] = succ;
    }
}

/*
* 函数名称：bpt_split_child
* 函数功能：根据pidx分裂一个满节点
*/
static void bpt_split_child(BPTree *parent, int pidx)
{
    BPTree *succ = bpt_alloc();
    BPTree *pred = parent->pointers[pidx];
    succ->is_leaf = pred->is_leaf;
    succ->nr_keys = split(CHILD_NUM - 1);
    pred->nr_keys = (CHILD_NUM - 1) - succ->nr_keys;
    for (int i = 0; i < succ->nr_keys; ++i)
    {
        succ->keys[i] = pred->keys[i + pred->nr_keys];
    }
    for (int i = 1; i <= succ->nr_keys; ++i)
    {
        succ->pointers[i] = pred->pointers[i + pred->nr_keys];
    }
    bpt_inject(parent, pidx, succ->keys[0], succ);
}

/*
* 函数名称：bpt_insert_nonfull
* 函数功能：value插入一个不满的叶子节点
*/
static void bpt_insert_nonfull(BPTree *bpt, uint64_t key, void *val)
{
    int kidx, pidx;
    while (!bpt->is_leaf)
    {
        bpt_index(bpt, key, &kidx, &pidx);
        if (((BPTree *)(bpt)->pointers[(pidx)])->nr_keys == CHILD_NUM - 1)
        {
            bpt_split_child(bpt, pidx);
            bpt_index(bpt, key, &kidx, &pidx);
        }
        assert(kidx < bpt->nr_keys);
        if (bpt->keys[kidx] == key)
        {
            return;
        }
        bpt = bpt->pointers[pidx];
        assert(bpt->nr_keys >= split(CHILD_NUM) - 1);
        assert(bpt->nr_keys < CHILD_NUM);
    }
    bpt_index(bpt, key, &kidx, &pidx);
    assert(kidx < bpt->nr_keys);
    if (bpt->keys[kidx] != key)
    {
        bpt_inject(bpt, pidx, key, val);
    }
}

/*
* 函数名称：bptree_insert
* 函数功能：树中插入一个value
*/
void bptree_insert(BPTree **root, uint64_t key, void *val)
{
    if ((*root)->nr_keys == CHILD_NUM - 1)
    {
        BPTree *new_root = bpt_alloc();
        new_root->is_leaf = 0;
        new_root->pointers[0] = *root;
        bpt_split_child(new_root, 0);
        *root = new_root;
        bpt_insert_nonfull(new_root, key, val);
    }
    else
    {
        bpt_insert_nonfull(*root, key, val);
    }
}

/*
* 函数名称：bpt_free
* 函数功能：释放一个节点
*/
static void bpt_free(BPTree *bpt)
{
    free(bpt->pointers);
    free(bpt->keys);
    free(bpt);
}

/*
* 函数名称：bptree_free
* 函数功能：释放整个树
*/
void bptree_free(BPTree *bpt)
{
    if (bpt->is_leaf)
    {
        bpt_free(bpt);
    }
    else
    {
        for (int i = 0; i <= bpt->nr_keys; ++i)
        {
            if (bpt->pointers[i])
            {
                bptree_free(bpt->pointers[i]);
            }
        }
        bpt_free(bpt);
    }
}

uint64_t *out[2];    //用来存放hash值
static BPTree *root; //树根

/*
* 函数名称：get_hash
* 函数功能：计算字符串的hash值
*/
uint64_t get_hash(char *str)
{
    MurmurHash3_x64_128(str, strlen(str), 31, out);
    return (uint64_t)out[0];
};

/*
* 函数名称：bplus_init_tree
* 函数功能：初始化树
*/
void bplus_init_tree()
{
    root = bptree_alloc(0, NULL);
}

/*
* 函数名称：bplus_insert_recoder
* 函数功能：树中插入字符串
*/
int bplus_insert_recoder(char *str)
{
    bptree_insert(&root, get_hash(str), str);
    return 1;
}

/*
* 函数名称：bplus_query_recoder
* 函数功能：查询字符串是否出现过
*/
int bplus_query_recoder(char *str)
{
    return bptree_exists(root, get_hash(str)) ? 1 : 0;
}
/*
* 函数名称：bplus_destroy_tree
* 函数功能：释放树
*/
void bplus_destroy_tree()
{
    bptree_free(root);
}
