#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ac_tree.h"
static Node *root;
static Node *ac_state;

static int n = 0;
static int k = 0;
static unsigned int node_num = 0;
Node *create_new_node()
{
    Node *_node = (Node *)malloc(sizeof(Node));
    _node->str = 0;
    _node->father = NULL;
    _node->child = NULL;
    _node->brother = NULL;
    _node->turn = NULL;
    _node->queue_next = NULL;
    _node->num = node_num++;
    return _node;
}

unsigned char get_str(char *str, short index)
{
    char _ch;
    int child_num = 1 << (node_bit_len);
    switch (index % child_num)
    {
    case 0:
        _ch = str[index / child_num] & 0b11000000;
        break;
    case 1:
        _ch = str[index / child_num] & 0b00110000;
        break;
    case 2:
        _ch = str[index / child_num] & 0b00001100;
        break;
    case 3:
        _ch = str[index / child_num] & 0b00000011;
        break;
    }
    return (_ch << (index % child_num * 2));
}

Node *search_child(Node *node, unsigned char _ch)
{
    node = node->child;
    while (node)
    {
        if ((node->str & 0b11000000) == _ch)
        {
            return node;
        }
        node = node->brother;
    }
    return NULL;
}

void init_tree()
{
    root = create_new_node();
    ac_state = root;
    return;
}

void output(Node *node, int line_num, int line_position)
{
    int k = 0;
    char out[512];
    for (; k < 512; k++)
    {
        out[k] = 0;
    }
    k = 510 * 4 + 3;
    while (node != root)
    {
        out[k / 4] = out[k / 4] | ((node->str & 0b11000000) >> k % 4 * 2);
        k--;
        node = node->father;
    }
    int len = (2043 - k) / 4;
    char *ch = &(out[511 - len]);
    printf("%10s %6d %6d\n", ch, line_num, (line_position + 1) / 4);
}



void insert_recoder(char *str)
{
    int i;
    Node *point = root;
    short len = strlen(str) * 8 / node_bit_len;
    for (i = 0; i < len; i++)
    {
        int j = 1;
        Node *point_father = point;

        point = point->child;
        while (point)
        {
            if ((point->str & 0b11000000) == (int)get_str(str, i))
            {
                j = 0;
                break;
            }
            if (!point->brother)
            {
                break;
            }
            point = point->brother;
        }
        if (j)
        {
            Node *new_node = create_new_node();
            new_node->str = get_str(str, i);
            new_node->father = point_father;
            if (point)
            {
                point->brother = new_node;
            }
            else
            {
                point_father->child = new_node;
            }
            point = new_node;
        }
    }
    point->str = point->str | 0b00000001;
}

Node *fount, *tail;
Node *_temp;
Node *get_node()
{
    if (fount)
    {
        _temp = fount;
        fount = fount->queue_next;
        //printf("%d\n",(unsigned int) _temp->num);
        return _temp;
    }
    return NULL;
}

void put_node(Node *node)
{
    if (!fount)
    {
        fount = node;
    }
    node->queue_next = NULL;
    tail->queue_next = node;
    tail = node;
}
static int j = 0;
void mk_turn()
{
    Node *node = fount;
    if (!node)
    {
        return;
    }
    fount = fount->queue_next;
    if (node != root)
    {
        node->turn = root;
        node->fail_num = root->num;
    }

    if (node != root)
    {
        Node *turn_node = node->father->turn;
        while (turn_node)
        {
            Node *_node = turn_node;
            _node = search_child(_node, (unsigned char)node->str & 0b11000000);
            if (_node)
            {
                //printf("%d\n", ++j);
                node->fail_num = _node->num;
                node->turn = _node;
                break;
            }
            else
            {
                turn_node = turn_node->turn;
            }
        }
    }
    if(node->brother){
        put_node(node->brother);
    };
    if (node->child)
    {
        put_node(node->child);
    }
    mk_turn();
}
BfsNode *brs_front, *bfs_tail;
void add_to_bfs_queue(Node *node){
    BfsNode *bfs_node = (BfsNode *)malloc(sizeof(BfsNode));
    bfs_node->node = node;
    bfs_node->next = NULL;
    bfs_tail->next = bfs_node;
    bfs_tail = bfs_node;
}

BfsNode *get_bfs_node(){
    Node *_temp = NULL;
    Node *bfs_temp = brs_front;
    if(brs_front){
        _temp = brs_front->node;
        brs_front = brs_front->next;
        free(bfs_temp);
    }
    return _temp;
}

void make_turn()
{
    brs_front = (BfsNode *)malloc(sizeof(BfsNode));
    bfs_tail = brs_front;
    brs_front->node = root;
    root->turn = NULL;
    mk_turn();
    root->turn = NULL;
}

void query_recoder(char *str, int line_num)
{
    int i;
    Node *child;
    short len = strlen(str) * 8 / node_bit_len;
    for (i = 0; i < len; i++)
    {
        if(i%4 == 3){
            printf("");
        }
        child = search_child(ac_state, get_str(str, i));
        if (child)
        {
            ac_state = child;
            if (ac_state->str & 0b00000001)
            {
                output(ac_state, line_num, i);
            }
            Node *re_node = ac_state;
            while (re_node->turn)
            {
                if (re_node->turn->str & 0b00000001)
                {
                    output(re_node->turn, line_num, i);
                }
                re_node = re_node->turn;
            }
            
        }
        else
        {
            if (ac_state->turn)
            {
                ac_state = ac_state->turn;
                i--;
            }
            
        }
    }
}