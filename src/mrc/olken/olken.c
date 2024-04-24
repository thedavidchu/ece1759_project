#include <assert.h>
#include <stdbool.h> // bool
#include <stdint.h>  // uint64_t
#include <stdio.h>
#include <stdlib.h>

#include "histogram/basic_histogram.h"
#include "olken/olken.h"
#include "tree/basic_tree.h"
#include "tree/sleator_tree.h"
#include "tree/types.h"
#include "lookup/parallel_hash_table.h"

bool
olken__init(struct OlkenReuseStack *me, const uint64_t max_num_unique_entries)
{
    if (me == NULL) {
        return false;
    }
    bool r = tree__init(&me->tree);
    if (!r) {
        goto tree_error;
    }
    // NOTE Using the g_direct_hash function means that we need to pass our
    //      entries as pointers to the hash table. It also means we cannot
    //      destroy the values at the pointers, because the pointers are our
    //      actual values!
    r = ParallelHashTable__init(&me->hash_table, 1 << 20);
    if (!r) {
        goto hash_table_error;
    }
    r = basic_histogram__init(&me->histogram, max_num_unique_entries);
    if (!r) {
        goto histogram_error;
    }
    me->current_time_stamp = 0;
    return true;

histogram_error:
    ParallelHashTable__destroy(&me->hash_table);
hash_table_error:
    tree__destroy(&me->tree);
tree_error:
    return false;
}

void
olken__access_item(struct OlkenReuseStack *me, EntryType entry)
{
    bool r = false;

    if (me == NULL) {
        return;
    }

    struct LookupReturn found = ParallelHashTable__lookup(&me->hash_table, entry);
    if (found.success) {
        uint64_t distance = tree__reverse_rank(&me->tree, (KeyType)found.timestamp);
        r = tree__sleator_remove(&me->tree, (KeyType)found.timestamp);
        assert(r && "remove should not fail");
        r = tree__sleator_insert(&me->tree, (KeyType)me->current_time_stamp);
        assert(r && "insert should not fail");
        r = ParallelHashTable__put_unique(&me->hash_table, entry, me->current_time_stamp);
        assert(r && "update should not fail");
        ++me->current_time_stamp;
        // TODO(dchu): Maybe record the infinite distances for Parda!
        basic_histogram__insert_finite(&me->histogram, distance);
    } else {
        r = ParallelHashTable__put_unique(&me->hash_table, entry, me->current_time_stamp);
        assert(r && "insert should not fail");
        tree__sleator_insert(&me->tree, (KeyType)me->current_time_stamp);
        ++me->current_time_stamp;
        basic_histogram__insert_infinite(&me->histogram);
    }
}

void
olken__print_histogram_as_json(struct OlkenReuseStack *me)
{
    if (me == NULL) {
        // Just pass on the NULL value and let the histogram deal with it. Maybe
        // this isn't very smart and will confuse future-me? Oh well!
        basic_histogram__print_as_json(NULL);
        return;
    }
    basic_histogram__print_as_json(&me->histogram);
}

void
olken__destroy(struct OlkenReuseStack *me)
{
    if (me == NULL) {
        return;
    }
    tree__destroy(&me->tree);
    ParallelHashTable__destroy(&me->hash_table);
    basic_histogram__destroy(&me->histogram);
    *me = (struct OlkenReuseStack){0};
}
