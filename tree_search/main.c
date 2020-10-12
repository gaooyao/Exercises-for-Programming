#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_file_reader.h"

#include "search_bplus.h"
#include "search_mtrie.h"
#include "search_radix.h"
#include "search_rawtrie.h"

int main()
{
    int i;
    char *str = NULL;
    FileHandler *file_dict;
    FileHandler *file_string;
    FileHandler *file_result;
    char *result_file_list[4] = {"result_bplus.txt", "result_rawtrie.txt", "result_mtrie.txt", "result_radix.txt"};
    char operate_list[4] = {'w', ' ', ' ', ' '}; //b:bplus;w:rawtrie;m:mtrie;d:radix;
    file_dict = open_file("dict.txt", "rb");
    file_string = open_file("string.txt", "rb");
    typedef void (*init_tree_function)();
    typedef int (*insert_recoder_function)(char *str);
    typedef int (*query_recoder_function)(char *str);
    typedef void (*destroy_tree_function)();
    init_tree_function init_tree;
    insert_recoder_function insert_recoder;
    query_recoder_function query_recoder;
    destroy_tree_function destroy_tree;
    for (i = 0; i < 4; i++)
    {
        switch (operate_list[i])
        {
        case 'b':
            init_tree = bplus_init_tree;
            insert_recoder = bplus_insert_recoder;
            query_recoder = bplus_query_recoder;
            destroy_tree = bplus_destroy_tree;
            file_result = open_file(result_file_list[0], "wb");
            break;
        case 'w':
            init_tree = rawtrie_init_tree;
            insert_recoder = rawtrie_insert_recoder;
            query_recoder = rawtrie_query_recoder;
            destroy_tree = rawtrie_destroy_tree;
            file_result = open_file(result_file_list[1], "wb");
            break;
        case 'm':
            init_tree = mtrie_init_tree;
            insert_recoder = mtrie_insert_recoder;
            query_recoder = mtrie_query_recoder;
            destroy_tree = mtrie_destroy_tree;
            file_result = open_file(result_file_list[2], "wb");
            break;
        case 'd':
            init_tree = radix_init_tree;
            insert_recoder = radix_insert_recoder;
            query_recoder = radix_query_recoder;
            destroy_tree = radix_destroy_tree;
            file_result = open_file(result_file_list[3], "wb");
            break;
        default:
            continue;
            break;
        }
        
        while (read_line(file_dict, &str))
        {
            if (!insert_recoder(str))
            {
                //printf("%s\n",str);
            };
        }
        while (read_line(file_string, &str))
        {
            if (query_recoder(str))
            {
                write_line(file_result, str);
            }
        }
        destroy_tree();
        close_file(file_result);
        reset_file(file_dict);
        reset_file(file_string);
    }
    close_file(file_dict);
    close_file(file_string);
    printf("\nProcess finished.\n");
    return 0;
}