#ifndef ac_tree_head
#define ac_tree_head

#define node_bit_len 2

typedef struct node{
    char str;
    struct node *father;
    struct node *child;
    struct node *brother;
    struct node *turn;
} Node;

void init_tree();
void insert_recoder(char *str);

#endif
