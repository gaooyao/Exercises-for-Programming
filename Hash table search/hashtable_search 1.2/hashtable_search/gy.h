//
//  gy.h
//  hashtable_search
//
//  Created by 杨恩 on 2020/9/22.
//

#ifndef gy_h
#define gy_h

HashNode* create_new_node(char* str);//创建新的hash节点
FileHandler* open_file(char* file_name, char* open_type);//通过文件接口打开新文件
int read_line(FileHandler* file_handler, char** str);//从文件中读取一行
int reset_file(FileHandler* file_handler);
int close_file(FileHandler* file_handler);//关闭文件
int manipulate_m(char*m,unsigned int k,int typ);
#endif gy_h
