#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "build_graph.h"

FileNode *file_queue_front;
FileNode *file_queue_tail;

void create_file_queue(char*file_name,char *file_path){

}

void match_link(char*file_name,char *file_path){

}

void travel_file(char *path,void(*operator)(char*file_name,char *file_path))
{
    printf("%s\n", path);
    DIR *dir;
    struct dirent *ptr;
    char dir_path[1000];
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
            if (ptr->d_name[file_name_len - 2] == 'm' && ptr->d_name[file_name_len - 1] == 'l')
            {
                operator(ptr->d_name, path);
                printf("d_name:%s/%s\n", path, ptr->d_name);
            }
            else
            {
                memset(dir_path, '\0', sizeof(dir_path));
                strcpy(dir_path, path);
                strcat(dir_path, "/");
                strcat(dir_path, ptr->d_name);
                travel_file(dir_path,create_file_queue);
            }
        }
    }
    closedir(dir);
}

void build_graph()
{
    file_queue_front = (FileNode*)malloc(sizeof(FileNode));
    file_queue_front->path = NULL;
    file_queue_tail = file_queue_front;
    char base_path[1000] = {0};
    getcwd(base_path, 1024);
    strcat(base_path, web_page_dir_path);
    travel_file(base_path);
}