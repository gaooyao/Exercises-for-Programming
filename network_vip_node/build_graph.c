#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "build_graph.h"
#include "config.h"

char base_path[4096] = {0};
int base_path_len = 0;
unsigned int file_num = 0;

FileNode *file_queue_front;
FileNode *file_queue_tail;

void create_file_queue(char *file_name, char *file_path)
{
    int file_name_len = strlen(file_name);
    if (file_name[file_name_len - 2] == 'm' && file_name[file_name_len - 1] == 'l')
    {
        file_num++;
        //printf("%s\n", file_name);
        if(!strcmp(file_name,"sichuan.shtml")){
            printf("we");
        }
        char *ch = (char *)malloc(strlen(file_path) + strlen(file_name) - base_path_len + 1);
        strcpy(ch, &file_path[base_path_len]);
        strcat(ch, dir_sp);
        strcat(ch, file_name);
        FileNode *file_node = (FileNode *)malloc(sizeof(FileNode));
        file_node->path = ch;
        file_node->next = NULL;
        file_queue_tail->next = file_node;
        file_queue_tail = file_node;
    }
}

void match_link(char *file_name, char *file_path)
{
}

void travel_file(char *path, void (*operator)(char *file_name, char *file_path))
{
    DIR *dir;
    struct dirent *ptr;
    char dir_path[4096];
    printf("%s\n",path);
    if ((dir = opendir(path)) == NULL)
    {
        return;
    }
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
        {
            continue;
        }
        else
        {
            int file_name_len = strlen(ptr->d_name);
            if (ptr->d_type == 8)
            {
                operator(ptr->d_name, path);
            }
            else if (ptr->d_type == 4)
            {
                memset(dir_path, '\0', sizeof(dir_path));
                strcpy(dir_path, path);
                strcat(dir_path, dir_sp);
                strcat(dir_path, ptr->d_name);
                travel_file(dir_path, create_file_queue);
            }
        }
    }
    closedir(dir);
}

void build_graph()
{
    file_queue_front = (FileNode *)malloc(sizeof(FileNode));
    file_queue_front->path = NULL;
    file_queue_tail = file_queue_front;
    getcwd(base_path, 4096);
    strcat(base_path, web_page_dir_path);
    base_path_len = strlen(base_path);
    travel_file(base_path, create_file_queue);
    printf("%d\n", file_num);
    getchar();
    getchar();
    getchar();
}