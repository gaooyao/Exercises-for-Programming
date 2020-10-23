#ifndef ac_tree_head
#define ac_tree_head

#define node_bit_len 2

typedef struct node{
    unsigned int num;
    unsigned int fail_num;
    char str;
    struct node *father;
    struct node *child;
    struct node *brother;
    struct node *turn;
    struct node *queue_next;
} Node;

typedef struct bfs_node{
    struct node *next;
    struct node *node;
} BfsNode;



void init_tree();
void insert_recoder(char *str);
void query_recoder(char *str,int line_num);
void make_turn();

#endif
