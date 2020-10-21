#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ac_tree.h"
static Node *root;

Node *create_new_node()
{
    Node *_node = (Node *)malloc(sizeof(Node));
    _node->str = 0;
    _node->father = NULL;
    _node->child = NULL;
    _node->brother = NULL;
    _node->turn = NULL;
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
    return;
}

static int n = 0;
static int k = 0;

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
            point = point->brother;
        }
        if (j)
        {
            Node *new_node = create_new_node();
            new_node->str = get_str(str, i);
            new_node->father = point_father;
            new_node->turn = root;
            if (point)
            {
                point->brother = new_node;
            }
            else
            {
                point_father->child = new_node;
            }
            Node *turn_node = new_node->father->turn;
            while (turn_node)
            {
                Node *_node = turn_node;
                _node = search_child(_node, get_str(str, i));
                if (_node)
                {
                    new_node->turn = _node;
                    break;
                }
                else
                {
                    turn_node = turn_node->turn;
                }
            }
            point = new_node;
        }
    }
    point->str = point->str | 0b00000001;
}