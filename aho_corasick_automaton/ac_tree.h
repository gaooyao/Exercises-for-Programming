#ifndef ac_tree_head
#define ac_tree_head

typedef struct node{
    char str;
    char next;
    char is_end;
    struct node *father;
    struct node *child;
    struct node *brother;
    struct node *turn_node;
} Node;

#endif