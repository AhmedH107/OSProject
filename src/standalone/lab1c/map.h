/* do not forget the guard against multiple includes */

#pragma once
#include <stdbool.h>
#include "../../lib/kernel/list.h"
#include <stdlib.h>

typedef char* value_t;
typedef int key_t;



typedef struct map_element
{
    value_t value;
    key_t key;
    struct list_elem elem;
} map_element;

typedef struct map
{
    /* listan med alla lagrade element */
    struct list content;
    /* räknare för vilken nyckel som är nästa lediga */
    int next_key;
    
} map;
 
void map_init(struct map* m);

key_t map_insert(struct map* m, value_t v);

value_t map_find(struct map* m, key_t k);

value_t map_remove(struct map* m, key_t k);

void map_for_each(struct map* m,
    void (*exec)(key_t k, value_t v, int aux),
    int aux);

void map_remove_if(struct map* m,
        bool (*cond)(key_t k, value_t v, int aux),
        int aux);





