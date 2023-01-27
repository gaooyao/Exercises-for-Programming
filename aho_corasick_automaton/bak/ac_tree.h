#ifndef ac_tree_head
#define ac_tree_head

#define node_bit_len 2


typedef struct out{
    char *str;
    unsigned int num;
    int line_1;
    int line_2;
    int line_3;
} Out;

typedef struct node{
    unsigned char str;
    struct node *father;
    struct node *child;
    struct node *brother;
    struct node *turn;
    struct out *out;
} Node;




void init_tree();
void insert_recoder(char *str);
void query_recoder(char *str,int line_num);
void make_turn();
void out_to_result();
void reset();

#endif
