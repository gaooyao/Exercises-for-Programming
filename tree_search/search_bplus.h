#ifndef search_bplus
#define search_bplus

#include <stdint.h>

#define CHILD_NUM 64

typedef struct bptree
{
    uint64_t *keys;
    void **pointers;
    uint16_t is_leaf : 1;
    uint16_t nr_keys : 15;
} BPTree;

BPTree *bptree_alloc(uint64_t key, void *val);
BPTree *bptree_exists(BPTree *bpt, uint64_t key);
void bptree_insert(BPTree **root, uint64_t key, void *val);
void bptree_free(BPTree *bpt);

void bplus_init_tree();
int bplus_insert_recoder(char *str);
int bplus_query_recoder(char *str);
void bplus_destroy_tree();

#endif