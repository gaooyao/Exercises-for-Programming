#include "search_bplus.h"
#include "murmurhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

struct BPlusTreeNode *Root;
int MaxChildNumber = 64;
uint64_t *out[2];
/** Destroy subtree whose root is Cur, By recursion */
void Destroy(BPlusTreeNode *Cur)
{
    if (Cur->isLeaf == true)
    {
        int i;
        for (i = 0; i < Cur->key_num; i++)
        {
            free(Cur->child[i]);
        }
    }
    else
    {
        int i;
        for (i = 0; i < Cur->key_num; i++)
            Destroy(Cur->child[i]);
    }
    free(Cur);
}

/** Interface: Called to destroy the B+tree */
void BPlusTree_Destroy()
{
    if (Root == NULL)
        return;
    Destroy(Root);
    Root = NULL;
}

/** Binary search to find the biggest child l that Cur->key[l] <= key */
int Binary_Search(BPlusTreeNode *Cur, int key)
{
    int l = 0, r = Cur->key_num;
    if (key < Cur->key[l])
        return l;
    if (Cur->key[r - 1] <= key)
        return r - 1;
    while (l < r - 1)
    {
        int mid = (l + r) >> 1;
        if (Cur->key[mid] > key)
            r = mid;
        else
            l = mid;
    }
    return l;
}
/** Find a leaf node that key lays in it
 *	modify indicates whether key should affect the tree
 */
BPlusTreeNode *Find(int key)
{
    BPlusTreeNode *Cur = Root;
    while (1)
    {
        if (Cur->isLeaf == true)
            break;
        if (key < Cur->key[0])
        {
            Cur = Cur->child[0];
        }
        else
        {
            Cur = Cur->child[Binary_Search(Cur, key)];
        }
    }
    return Cur;
}
/** Create a new B+tree Node */
BPlusTreeNode *New_BPlusTreeNode()
{
    struct BPlusTreeNode *p = (struct BPlusTreeNode *)malloc(sizeof(struct BPlusTreeNode));
    p->isRoot = false;
    p->isLeaf = false;
    p->key_num = 0;
    p->child[0] = NULL;
    p->father = NULL;
    p->next = NULL;
    p->last = NULL;
    return p;
}
void Insert(BPlusTreeNode *Cur, int key, BPlusTreeRecorder *value);
void Split(BPlusTreeNode *Cur)
{
    // copy Cur(Mid .. MaxChildNumber) -> Temp(0 .. Temp->key_num)
    BPlusTreeNode *Temp = New_BPlusTreeNode();
    BPlusTreeNode *ch;
    int Mid = MaxChildNumber >> 1;
    Temp->isLeaf = Cur->isLeaf; // Temp's depth == Cur's depth
    Temp->key_num = MaxChildNumber - Mid;
    int i;
    for (i = Mid; i < MaxChildNumber; i++)
    {
        Temp->child[i - Mid] = Cur->child[i];
        Temp->key[i - Mid] = Cur->key[i];
        if (!Temp->isLeaf)
        {
            ch = (BPlusTreeNode *)Temp->child[i - Mid];
            ch->father = Temp;
        }
    }
    // Change Cur
    Cur->key_num = Mid;
    // Insert Temp
    if (Cur->isRoot)
    {
        // Create a new Root, the depth of Tree is increased
        Root = New_BPlusTreeNode();
        Root->key_num = 2;
        Root->isRoot = true;
        Root->key[0] = Cur->key[0];
        Root->child[0] = Cur;
        Root->key[1] = Temp->key[0];
        Root->child[1] = Temp;
        Cur->father = Temp->father = Root;
        Cur->isRoot = false;
        if (Cur->isLeaf)
        {
            Cur->next = Temp;
            Temp->last = Cur;
        }
    }
    else
    {
        // Try to insert Temp to Cur->father
        Temp->father = Cur->father;
        Insert(Cur->father, Cur->key[Mid], (void *)Temp);
    }
}
/** Insert (key, value) into Cur, if Cur is full, then split it to fit the definition of B+tree */
void Insert(BPlusTreeNode *Cur, int key, BPlusTreeRecorder *value)
{
    int i, ins;
    if (key < Cur->key[0])
        ins = 0;
    else
        ins = Binary_Search(Cur, key) + 1;
    for (i = Cur->key_num; i > ins; i--)
    {
        Cur->key[i] = Cur->key[i - 1];
        Cur->child[i] = Cur->child[i - 1];
    }
    Cur->key_num++;
    Cur->key[ins] = key;
    Cur->child[ins] = (void* )value;
    if (Cur->isLeaf == false)
    { // make links on leaves
        BPlusTreeNode *firstChild = (BPlusTreeNode *)(Cur->child[0]);
        if (firstChild->isLeaf == true)
        { // which means value is also a leaf as child[0]
            BPlusTreeNode *temp = (BPlusTreeNode *)(value);
            if (ins > 0)
            {
                BPlusTreeNode *prevChild;
                BPlusTreeNode *succChild;
                prevChild = (BPlusTreeNode *)Cur->child[ins - 1];
                succChild = prevChild->next;
                prevChild->next = temp;
                temp->next = succChild;
                temp->last = prevChild;
                if (succChild != NULL)
                    succChild->last = temp;
            }
            else
            {
                // do not have a prevChild, then refer next directly
                // updated: the very first record on B+tree, and will not come to this case
                temp->next = Cur->child[1];
            }
        }
    }
    if (Cur->key_num == MaxChildNumber) // children is full
        Split(Cur);
}
/** Interface: Insert (key, value) into B+tree */
int BPlusTree_Insert(int key, BPlusTreeRecorder* value)
{
    BPlusTreeNode *Leaf = Find(key);
    int i = Binary_Search(Leaf, key);
    if (Leaf->key[i] == key){
        BPlusTreeRecorder *rec = (BPlusTreeRecorder *)Leaf->child[i];
        while(rec->next){
            rec = rec->next;
        }
        rec->next = value;
        return true;
    }  
    Insert(Leaf, key, value);
    return true;
}
int get_hash(char *str)
{
    MurmurHash3_x64_128(str, strlen(str), 31, out);
    return (int)out[0] & 0x7fffffff;
};

void bplus_init_tree()
{
    BPlusTree_Destroy();
    Root = New_BPlusTreeNode();
    Root->isRoot = true;
    Root->isLeaf = true;
}

int bplus_insert_recoder(char *str)
{
    char* recorder_str = (char *)malloc(strlen(str));
    strcpy(recorder_str, str);
    BPlusTreeRecorder* new_recorder = (BPlusTreeRecorder *)malloc(sizeof(BPlusTreeRecorder));
    new_recorder->str = recorder_str;
    new_recorder->next = NULL;
    if(!BPlusTree_Insert(get_hash(str), new_recorder)){
        
    }
    return 1;
}

int bplus_query_recoder(char *str)
{
    int key;
    key = get_hash(str);
    BPlusTreeNode *Leaf = Find(key);
    int i;
    for (i = 0; i < Leaf->key_num; i++)
    {
        if (Leaf->key[i] == key)
        {
            return 1;
        }
    }
    return 0;
}
void bplus_destroy_tree()
{
    //BPlusTree_Destroy();
    printf("destroy b\n");
}
