#ifndef search_rawtrie
#define search_rawtrie

void rawtrie_init_tree();
int rawtrie_insert_recoder(char *str);
int rawtrie_query_recoder(char *str);
void rawtrie_destroy_tree();

#endif