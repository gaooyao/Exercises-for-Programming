#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

typedef struct SmallStr {
    char str[10];
}SmallStr;
typedef struct BigStr {
    char str[100];
}BigStr;

typedef struct HashNode {
    SmallStr* small_str;
    BigStr* big_str;
    struct HashNode* next;
}HashNode;



unsigned int BKDRHash(char* str) {
    unsigned int seed = 31; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    while (*str)
    {
        
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}


int get_hash(char* str) {
    return BKDRHash(str) % 23767;
}

void create_node_str(HashNode* node, char* str) {
    if (strlen(str) < 10) {
        SmallStr str_node;
        strcpy(str_node.str, str);
        printf("%s\n", str_node.str);
        node->small_str = &str_node;
        printf("666%s\n", node->small_str->str);
    }
    else {
        BigStr str_node;
        strcpy(str_node.str, str);
        node->big_str = &str_node;
    }
}


int main() {
    HashNode hash_table[23767] = { 0 };
    int list[256][256][256] = {0};
    FILE* string_file = NULL;
    FILE* dict_file = NULL;
    char str[99] = { 0 };
    string_file = fopen("string.txt", "r");
    dict_file = fopen("dict.txt", "r");

    if (!string_file) {
        printf("打开string.txt文件失败！");
        return 0;
    }
    while (fscanf(string_file, "%s", str) != -1) {
        unsigned short int first_char = str[0] & 0xff;
        unsigned short int second_char = str[1] & 0xff;
        unsigned short int third_char = str[2] & 0xff;
        //printf("%s %d %d %d\n", str, first_char, second_char, third_char);
        list[first_char][second_char][third_char] = 1;

        // printf("%d\n", get_hash(str));

        /*

        int hash_key = get_hash(str);
        if (!(hash_table[hash_key].small_str) && !(hash_table[hash_key].big_str)) {
            printf("bbbb%s\n", (hash_table[hash_key].small_str->str));
            create_node_str(&hash_table[hash_key], str);
            //printf("%s\n", hash_table[hash_key].small_str);
            printf("ffff%s\n\n", hash_table[hash_key].small_str->str);
        }
        else {
            HashNode new_node = { NULL,NULL,NULL };
            create_node_str(&new_node, str);
            HashNode* node_pointer = &hash_table[hash_key];
            while (node_pointer->next) {
                node_pointer = node_pointer->next;
            }
            node_pointer->next = &new_node;
        }

        */
    }
    int t = 0;
    int k = 0;
    while (fscanf(dict_file, "%s", str) != -1) {
        t++;
        unsigned short int first_char = str[0] & 0xff;
        unsigned short int second_char = str[1] & 0xff;
        unsigned short int third_char = str[2] & 0xff;
        // printf("%s %d %d\n", str, first_char, second_char);
        if (list[first_char][second_char][third_char]) {
            k++;

            /*
            int hash_key = get_hash(str);
            HashNode* node_pointer = &(hash_table[hash_key]);
            while (node_pointer) {
                if (node_pointer->small_str != NULL) {
                    printf("%s\n", node_pointer->small_str->str);
                    printf("%s %s %d\n", node_pointer->small_str->str, str, strcmp(node_pointer->small_str->str, str));
                    //printf("%d\n", strcmp(node_pointer->small_str->str, str));
                }


                
                //printf("find %s\n", node_pointer->small_str->str);
                if (node_pointer->small_str!=NULL && !strcmp(node_pointer->small_str->str,str)) {
                    printf("ssssss %s\n", str);
                    break;
                }
                else if(node_pointer->big_str != NULL && !strcmp(node_pointer->big_str->str, str)) {
                    printf("bbbbbb %s\n", str);
                    break;
                }
                node_pointer = node_pointer->next;
            }
            */
        }
    }
    printf("%d %d\n", t, k);

    return 0;

}
