#ifndef ac_tree_head
#define ac_tree_head

typedef struct out
{
    char *str;
    unsigned int num;
    int line_1;
    int line_2;
    int line_3;
} Out;

typedef struct node
{
    char str;
    unsigned short is_end;
    struct node *father;
    struct node *child;
    struct node *brother;
    struct node *turn;
    struct out *out;
} Node;

void init_tree();
void insert_recoder(char *str);
void query_recoder(char *str, int line_num);
void make_turn();
void out_to_result();

#endif
