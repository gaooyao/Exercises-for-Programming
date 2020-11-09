#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

void read_file_fist(char *path)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir = opendir(path)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if (ptr->d_type == 8) ///file
            printf("d_name:%s/%s\n", path, ptr->d_name);
        else if (ptr->d_type == 10) ///link file
            printf("d_name:%s/%s\n", path, ptr->d_name);
        else if (ptr->d_type == 4) ///dir
        {
            memset(base, '\0', sizeof(base));
            strcpy(base, path);
            strcat(base, "/");
            strcat(base, ptr->d_name);
            readFileList(base);
        }
    }
    closedir(dir);
}

void build_graph()
{
    DIR *dir;
    char basePath[1000];
    memset(basePath, '\0', sizeof(basePath));
    getcwd(basePath, 999);
    printf("the current dir is : %s\n", basePath);

    memset(basePath, '\0', sizeof(basePath));
    strcpy(basePath, "./XL");
    read_file_fist(basePath);
}