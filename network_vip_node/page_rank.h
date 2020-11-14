#ifndef page_rank_head
#define page_rank_head

#define MAX_ELL_NUM 20

typedef struct page_node
{
    char* path;
    int out_link_num;
    float out_link_weight;
    int *out_link_list;
    int next_out_index;
    int next_out_id;
} PageNode;

void page_rank();

#endif