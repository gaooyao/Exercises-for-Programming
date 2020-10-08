#ifndef search_bplus
#define search_bplus

void bplus_init_tree();
int bplus_insert_recoder(char *str);
int bplus_query_recoder(char *str);
void bplus_destroy_tree();

#endif