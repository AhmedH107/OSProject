#include <stddef.h>

#include "plist.h"
#include <stddef.h>

struct pmap p_container;

void p_map_init()
{

    list_init(&p_container.content); // must be & because it needs adress of content, cringe.
    p_container.next_key = 2;
}

key_t p_map_insert(pvalue_t v)
{
    struct pmap_element *elem = malloc(sizeof(struct pmap_element));
    if (elem != NULL)
    {
        elem->key = p_container.next_key;
        // If something bricks check if next_key overflows
        elem->value = v;
        list_push_back(&p_container.content, &elem->elem); // Insert into the linked list. Elem = node, *prev, *next
        p_container.next_key++;

        return elem->key;
    };
    return -1;
}

pvalue_t p_map_find(key_t k)
{
    // Iterate over each element in the list.
    for (struct list_elem *e = list_begin(&p_container.content);
         e != list_end(&p_container.content);
         e = list_next(e))
    {

        // Get the pmap_element struct that contains this list element.
        struct pmap_element *elem = list_entry(e, struct pmap_element, elem);

        // Check if this element's key matches the search key.
        if (elem->key == k)
        {
            return elem->value;
        }
    }

    // If no matching key is found, return NULL.
    return NULL;
}

pvalue_t p_map_remove(key_t k)
{
    for (struct list_elem *e = list_begin(&p_container.content);
         e != list_end(&p_container.content);
         e = list_next(e))
    {

        // Get the pmap_element struct that contains this list element.
        struct pmap_element *elem = list_entry(e, struct pmap_element, elem);

        // Check if this element's key matches the search key.
        if (elem->key == k)
        {
            list_remove(e);
            procInfo *pvalue_t = elem->value;
            free(elem);
            return pvalue_t;
        }
    }

    return NULL;
}

void p_map_for_each(
    void (*exec)(key_t k, pvalue_t v, int aux),
    int aux)
{

    for (struct list_elem *e = list_begin(&p_container.content);
         e != list_end(&p_container.content);
         e = list_next(e))
    {

        // Get the pmap_element struct that contains this list element.
        struct pmap_element *elem = list_entry(e, struct pmap_element, elem);

        exec(elem->key, elem->value, aux);
    }
};

void p_print(key_t k __attribute__((unused)),
             pvalue_t v,
             int aux __attribute__((unused)))
{
    pvalue_t pv = (pvalue_t)v;
    printf("parent=%d, free=%s, exitStatus=%d, exitSema=%p\n",
           pv->parent,
           pv->free ? "true" : "false",
           pv->exitStatus,
           (void *)&pv->exitSema);
}

void plistExitStatus(int pid)
{
    for (struct list_elem *e = list_begin(&p_container.content);
         e != list_end(&p_container.content);
         e = list_next(e))
    {

        // Get the pmap_element struct that contains this list element.
        struct pmap_element *elem = list_entry(e, struct pmap_element, elem);

        // Check if this element's key matches the search key.
        if (elem->key == pid)
        {
            elem->value->exitStatus = pid; // may allah strike me
        }
    }
}

void p_map_remove_if(
    bool (*cond)(key_t k, pvalue_t v, int aux),
    int aux)
{
    struct list_elem *e = list_begin(&p_container.content);
    while (e != list_end(&p_container.content))
    {
        struct list_elem *next = list_next(e);
        struct pmap_element *elem = list_entry(e, struct pmap_element, elem);

        if (cond(elem->key, elem->value, aux))
        {

            list_remove(e);
            free(elem);
        }

        e = next;
    }
}

#include "flist.h"
