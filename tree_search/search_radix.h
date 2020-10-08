#ifndef search_radix
#define search_radix

void radix_init_tree();
int radix_insert_recoder(char *str);
int radix_query_recoder(char *str);
void radix_destroy_tree();

#endif