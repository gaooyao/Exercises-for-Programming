#ifndef build_graph_head
#define build_graph_head

typedef struct file_node{
    char *path;
    struct file_node *next;
} FileNode;

void build_graph();

#endif