#include "search_bplus.h"
#include "murmurhash.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "search_bplus.h"

#define bpt_next pointers[0]

#define BPT_P(bpt, pidx) ((BPTree *)(bpt)->pointers[(pidx)])
#define BPT_PREF(bpt, pidx) ((BPTree **)&((*bpt)->pointers[(pidx)]))

static BPTree *bpt_alloc()
{
    BPTree *bpt = malloc(sizeof(BPTree));
    if (!bpt)
    {
        return NULL;
    }

    bpt->keys = malloc(sizeof(uint64_t) * (ORDER - 1));
    if (!bpt->keys)
    {
        goto release0;
    }

    bpt->pointers = calloc(ORDER, sizeof(void *));
    if (!bpt->pointers)
    {
        goto release1;
    }

    bpt->is_leaf = 1;
    bpt->nr_keys = 0;
    bpt->bpt_next = NULL;
    return bpt;

release1:
    free(bpt->keys);
release0:
    free(bpt);
    abort();
    return NULL;
}

/*
 * Create a B+ tree with one mapping in the root.
 */
BPTree *bptree_alloc(uint64_t key, void *val)
{
    BPTree *bpt = bpt_alloc();
    bpt->nr_keys = 1;
    bpt->keys[0] = key;
    bpt->pointers[1] = val;
    return bpt;
}

/*
 * Halve an index with adjustment for odd numbers.
 */
static inline int split(int x)
{
    return (x >> 1) + (x & 1);
}

/*
 * Find the key index for the given key, assuming that the key is in the range
 * [bpt.min, bpt.max) ((-inf, bpt.min) and [bpt.max, inf) must be handled
 * separately). 
 */
static int bpt_bisect(BPTree *bpt, uint64_t key)
{
    int low = 0;
    int high = bpt->nr_keys - 2;
    int mid = low + ((high - low) / 2);
    while (!(bpt->keys[mid] <= key && key < bpt->keys[mid + 1]))
    {
        assert(low < high);
        assert(bpt->keys[mid] != bpt->keys[mid + 1]);
        if (bpt->keys[mid] < key)
        {
            low = mid;
        }
        else if (bpt->keys[mid] > key)
        {
            high = mid - 1;
        }
        else
        {
            return mid;
        }
        mid = low == mid ? (mid + 1) : low + ((high - low) / 2);
    }
    return mid;
}

/*
 * Find the key and pointer indices of a search key.
 */
static void bpt_index(BPTree *bpt, uint64_t key, int *k, int *p)
{
    assert(bpt->nr_keys > 0);
    const int last_idx = bpt->nr_keys - 1;
    if (key < bpt->keys[0])
    {
        *k = *p = 0;
    }
    else if (key >= bpt->keys[last_idx])
    {
        *k = last_idx;
        *p = last_idx + 1;
    }
    else
    {
        *k = bpt_bisect(bpt, key);
        *p = *k + 1;
    }
}

/*
 * Find the leaf which contains the start of the range [key, inf).
 */
BPTree *bptree_search(BPTree *bpt, uint64_t key)
{
    int kidx, pidx;
    while (!bpt->is_leaf)
    {
        bpt_index(bpt, key, &kidx, &pidx);
        bpt = bpt->pointers[pidx];
        assert(bpt);
        assert(bpt->nr_keys >= split(ORDER) - 1);
    }
    return bpt;
}

/*
 * Finds the successor leaf for the given node.
 */
BPTree *bptree_next(BPTree *bpt)
{
    while (!bpt->is_leaf)
    {
        bpt = bpt->pointers[0];
    }
    return bpt->bpt_next;
}

/*
 * Find the leaf which contains the given key, or return NULL.
 */
BPTree *bptree_exists(BPTree *bpt, uint64_t key)
{
    int kidx, pidx;
    bpt = bptree_search(bpt, key);
    bpt_index(bpt, key, &kidx, &pidx);
    return (bpt && bpt->keys[kidx] == key) ? bpt : NULL;
}

/*
 * Key/value lookup. 
 */
void *bptree_lookup(BPTree *bpt, uint64_t key)
{
    int kidx, pidx;
    bpt = bptree_search(bpt, key);
    if (bpt)
    {
        bpt_index(bpt, key, &kidx, &pidx);
        return bpt->keys[kidx] == key ? bpt->pointers[pidx] : NULL;
    }
    else
    {
        return NULL;
    }
}

/*
 * Update the value of an existing key.
 */
void bptree_modify(BPTree *bpt, uint64_t key, void *val)
{
    int kidx, pidx;
    bpt = bptree_search(bpt, key);
    if (bpt)
    {
        bpt_index(bpt, key, &kidx, &pidx);
        if (bpt->keys[kidx] == key)
        {
            bpt->pointers[pidx] = val;
        }
    }
}

/*
 * Add an entry into a nonfull node after pidx.
 */
static void bpt_inject(BPTree *bpt, int pidx, uint64_t key, void *val)
{
    assert(bpt->nr_keys < ORDER - 1);
    for (int i = bpt->nr_keys; i > pidx; --i)
    {
        bpt->pointers[i + 1] = bpt->pointers[i];
    }
    bpt->pointers[pidx + 1] = val;
    for (int i = bpt->nr_keys - 1; i >= pidx; --i)
    {
        bpt->keys[i + 1] = bpt->keys[i];
    }
    bpt->keys[pidx] = key;
    ++bpt->nr_keys;

    BPTree *pred = bpt->pointers[pidx];
    if (!bpt->is_leaf && pred && pred->is_leaf)
    {
        BPTree *succ = bpt->pointers[pidx + 1];
        succ->bpt_next = pred->bpt_next;
        pred->bpt_next = succ;
    }
}

/*
 * Split a full child into two nodes about the median key.
 */
static void bpt_split_child(BPTree *parent, int pidx)
{
    BPTree *succ = bpt_alloc();
    BPTree *pred = parent->pointers[pidx];
    succ->is_leaf = pred->is_leaf;
    succ->nr_keys = split(ORDER - 1);
    pred->nr_keys = (ORDER - 1) - succ->nr_keys;

    for (int i = 0; i < succ->nr_keys; ++i)
    {
        succ->keys[i] = pred->keys[i + pred->nr_keys];
    }
    for (int i = 1; i <= succ->nr_keys; ++i)
    {
        succ->pointers[i] = pred->pointers[i + pred->nr_keys];
    }
    bpt_inject(parent, pidx, succ->keys[0], succ);
}

/*
 * Insert an entry into a nonfull parent.
 */
static void bpt_insert_nonfull(BPTree *bpt, uint64_t key, void *val)
{
    int kidx, pidx;
    while (!bpt->is_leaf)
    {
        bpt_index(bpt, key, &kidx, &pidx);
        if (BPT_P(bpt, pidx)->nr_keys == ORDER - 1)
        {
            bpt_split_child(bpt, pidx);
            bpt_index(bpt, key, &kidx, &pidx);
        }

        assert(kidx < bpt->nr_keys);
        if (bpt->keys[kidx] == key)
        {
            return;
        }

        bpt = bpt->pointers[pidx];
        assert(bpt->nr_keys >= split(ORDER) - 1);
        assert(bpt->nr_keys < ORDER);
    }

    bpt_index(bpt, key, &kidx, &pidx);
    assert(kidx < bpt->nr_keys);
    if (bpt->keys[kidx] != key)
    {
        bpt_inject(bpt, pidx, key, val);
    }
}

/*
 * Perform inserts, splitting the root node if necessary.
 */
void bptree_insert(BPTree **root, uint64_t key, void *val)
{
    if ((*root)->nr_keys == ORDER - 1)
    {
        BPTree *new_root = bpt_alloc();
        new_root->is_leaf = 0;
        new_root->pointers[0] = *root;
        bpt_split_child(new_root, 0);
        *root = new_root;
        bpt_insert_nonfull(new_root, key, val);
    }
    else
    {
        bpt_insert_nonfull(*root, key, val);
    }
}

static void *bpt_delete(BPTree *bpt, uint64_t key);

/*
 * Remove a key-value pair contained within a node.
 */
static void bpt_eject(BPTree *bpt, int kidx, int pidx)
{
    for (int i = kidx; i < bpt->nr_keys; ++i)
    {
        bpt->keys[i] = bpt->keys[i + 1];
    }
    for (int i = pidx; i <= bpt->nr_keys; ++i)
    {
        bpt->pointers[i] = bpt->pointers[i + 1];
    }
    BPTree *succ = bpt->pointers[pidx];
    if (!bpt->is_leaf && pidx > 0 && succ && succ->is_leaf)
    {
        BPTree *pred = bpt->pointers[pidx - 1];
        pred->bpt_next = succ;
    }
    --bpt->nr_keys;
}

/*
 * Move the last key in the left donor into the parent at kidx.
 */
static void bpt_rotate_right(BPTree *parent, int kidx, int donor_pidx)
{
    BPTree *donor = BPT_P(parent, donor_pidx);
    uint64_t kprime = donor->keys[donor->nr_keys - 1];
    parent->keys[kidx] = kprime;
    void *val = bpt_delete(BPT_P(parent, donor_pidx), kprime);
    bpt_insert_nonfull(BPT_P(parent, donor_pidx + 1), kprime, val);
}

/*
 * Move the second key in the right donor into the parent at kidx.
 */
static void bpt_rotate_left(BPTree *parent, int kidx, int donor_pidx)
{
    BPTree *donor = BPT_P(parent, donor_pidx);
    uint64_t kprime = donor->keys[0];
    parent->keys[kidx] = donor->keys[1];
    void *val = bpt_delete(BPT_P(parent, donor_pidx), kprime);
    bpt_insert_nonfull(BPT_P(parent, donor_pidx - 1), kprime, val);
}

/*
 * Free a bptree structure without touching any of its data.
 */
static void bpt_free(BPTree *bpt)
{
    free(bpt->pointers);
    free(bpt->keys);
    free(bpt);
}

/*
 * Merge parent.kidx0 and parent.kidx1 together.
 */
static void bpt_merge(BPTree *parent, int kidx1, int pidx1)
{
    BPTree *pred = parent->pointers[pidx1 - 1];
    BPTree *succ = parent->pointers[pidx1];
    assert(pred->nr_keys >= split(ORDER) - 1);
    assert(succ->nr_keys >= split(ORDER) - 1);
    assert(succ->nr_keys + pred->nr_keys <= ORDER - 1);
    assert(succ->is_leaf || succ->pointers[0] == NULL);
    bpt_eject(parent, kidx1, pidx1);
    for (int i = 0; i < succ->nr_keys; ++i)
    {
        bpt_inject(pred, pred->nr_keys, succ->keys[i],
                   succ->pointers[i + 1]);
    }
    bpt_free(succ);
}

static void *bpt_delete_noindex(BPTree *bpt, uint64_t key,
                                int kidx, int pidx);

/*
 * Perform deletions on subtrees.
 */
static void *bpt_delete(BPTree *bpt, uint64_t key)
{
    int kidx, pidx;
    bpt_index(bpt, key, &kidx, &pidx);
    return bpt_delete_noindex(bpt, key, kidx, pidx);
}

/*
 * Perform deletions using the provided indices for victim nodes.
 */
static void *bpt_delete_noindex(BPTree *bpt, uint64_t key,
                                int kidx, int pidx)
{
    void *val = NULL;
    int match = bpt->keys[kidx] == key;
    if (match && bpt->is_leaf)
    {
        val = bpt->pointers[pidx];
        bpt_eject(bpt, kidx, pidx);
    }
    else if (match && !bpt->is_leaf)
    {
        assert(bpt->nr_keys >= 1);
        assert(pidx <= bpt->nr_keys);
        BPTree *curr = BPT_P(bpt, pidx);
        BPTree *pred = pidx == 0 ? NULL : BPT_P(bpt, pidx - 1);
        BPTree *succ = pidx == bpt->nr_keys ? NULL : BPT_P(bpt, pidx + 1);
        if (curr->nr_keys >= split(ORDER))
        {
            val = bpt_delete(BPT_P(bpt, pidx), key);
            bpt->keys[kidx] = BPT_P(bpt, pidx)->keys[0];
        }
        else if (pred && pred->nr_keys >= split(ORDER))
        {
            bpt_rotate_right(bpt, kidx, pidx - 1);
            val = bpt_delete(BPT_P(bpt, pidx), key);
        }
        else if (succ && succ->nr_keys >= split(ORDER))
        {
            assert(!pred || pred->nr_keys == split(ORDER) - 1);
            bpt_rotate_left(bpt, kidx + 1, pidx + 1);
            val = bpt_delete(BPT_P(bpt, pidx), key);
            bpt->keys[kidx] = BPT_P(bpt, pidx)->keys[0];
        }
        else if (pred)
        {
            bpt_merge(bpt, kidx, pidx);
            val = bpt_delete(BPT_P(bpt, pidx - 1), key);
        }
        else if (succ)
        {
            bpt_merge(bpt, kidx + 1, pidx + 1);
            val = bpt_delete(BPT_P(bpt, pidx), key);
        }
        else
        {
            assert(!"Internal node key deletion failed!");
        }
    }
    else if (!bpt->is_leaf)
    {
        BPTree *rhs = NULL, *lhs = NULL;
        BPTree *child = BPT_P(bpt, pidx);
        if (!child)
        {
            return NULL;
        }

        if (child->nr_keys == split(ORDER) - 1)
        {
            int lhs_donor = pidx - 1 >= 0 &&
                            (lhs = bpt->pointers[pidx - 1]) &&
                            lhs->nr_keys >= split(ORDER);
            int rhs_donor = pidx + 1 <= bpt->nr_keys &&
                            (rhs = bpt->pointers[pidx + 1]) &&
                            rhs->nr_keys >= split(ORDER);
            if (lhs_donor)
            {
                bpt_rotate_right(bpt, kidx, pidx - 1);
            }
            else if (rhs_donor)
            {
                bpt_rotate_left(bpt, kidx, pidx + 1);
            }
            else if (rhs)
            {
                bpt_merge(bpt, kidx + 1, pidx + 1);
            }
            else if (lhs)
            {
                bpt_merge(bpt, kidx, pidx);
            }
            else
            {
                assert(!"Internal node is degenerate!");
                return NULL;
            }
        }

        val = bpt_delete(BPT_P(bpt, pidx), key);
    }

    return val;
}

/*
 * Remove a key, restructuring the tree as needed.
 */
void *bptree_delete(BPTree **bpt, uint64_t key)
{
    void *val = NULL;
    BPTree *root = *bpt;
    int kidx, pidx;
    bpt_index(root, key, &kidx, &pidx);
    int match = root->keys[kidx] == key;

    if (root->is_leaf)
    {
        if (match)
        {
            val = root->pointers[pidx];
            if (root->nr_keys == 1)
            {
                root->keys[kidx] = 0;
                root->pointers[pidx] = NULL;
            }
            else
            {
                bpt_eject(root, kidx, pidx);
            }
        }
    }
    else
    {
        if (root->nr_keys == 1)
        {
            assert(root->pointers[0] && root->pointers[1]);
            bpt_merge(root, 0, 1);
            *bpt = root->pointers[0];
            bpt_free(root);
            root = *bpt;
            val = bpt_delete(root, key);
        }
        else
        {
            val = bpt_delete_noindex(root, key, kidx, pidx);
        }
    }
    return val;
}

void bptree_free(BPTree *bpt)
{
    if (bpt->is_leaf)
    {
        bpt_free(bpt);
    }
    else
    {
        for (int i = 0; i <= bpt->nr_keys; ++i)
        {
            if (bpt->pointers[i])
            {
                bptree_free(bpt->pointers[i]);
            }
        }
        bpt_free(bpt);
    }
}

uint64_t *out[2];
static BPTree *root;
uint64_t get_hash(char *str)
{
    MurmurHash3_x64_128(str, strlen(str), 31, out);
    return (uint64_t)out[0];
};

void bplus_init_tree()
{
    root = bptree_alloc(0, NULL);
}

int bplus_insert_recoder(char *str)
{
    char *recorder_str = (char *)malloc(strlen(str));
    strcpy(recorder_str, str);
    BPlusTreeRecorder *new_recorder = (BPlusTreeRecorder *)malloc(sizeof(BPlusTreeRecorder));
    new_recorder->str = recorder_str;
    new_recorder->next = NULL;
    bptree_insert(&root, get_hash(str), new_recorder);
    return 1;
}

int bplus_query_recoder(char *str)
{
    return bptree_exists(root, get_hash(str)) ? 1 : 0;
}
void bplus_destroy_tree()
{
    bptree_free(root);
}
