//
//  gy.h
//  hashtable_search
//
//  Created by 杨恩 on 2020/9/22.
//

#ifndef gy_h
#define gy_h

#include "gy.cpp"

FILE* open_file(char* file_name, char* open_type);
HashNode* create_new_node(char* str);

FileHandler* open_fileb(char* file_name,char* open_type);
int read_line(FileHandler* file_handler, char* str);
int write_line(FileHandler* file_handler, char* str);
int close_file(FileHandler* file_handler);


#endif gy_h