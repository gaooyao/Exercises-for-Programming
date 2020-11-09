#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

void read_file_list(char *path)
{
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
        else if (ptr->d_type == 8) ///file
            printf("d_name:%s/%s\n", path, ptr->d_name);
        else if (ptr->d_type == 10) ///link file
            printf("d_name:%s/%s\n", path, ptr->d_name);
        else if (ptr->d_type == 4) ///dir
        {
            memset(dir_path, '\0', sizeof(dir_path));
            strcpy(dir_path, path);
            strcat(dir_path, "/");
            strcat(dir_path, ptr->d_name);
            read_file_list(dir_path);
        }
    }
    closedir(dir);
}

void build_graph()
{
    char base_path[1000] = {0};
    getcwd(base_path, 1024);
    printf("%s\n", web_page_dir_path);
    printf("%s\n", base_path);
    strcat(base_path, web_page_dir_path);
    printf("%s\n", base_path);
    read_file_list(base_path);
}