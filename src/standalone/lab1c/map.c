#include "map.h"


void map_init(struct map *m) {
    list_init(&m->content);  //must be & because it needs adress of content, cringe.
    m->next_key = 0;
}

key_t map_insert(struct map *m, value_t v) {
    struct map_element *elem = malloc(sizeof(struct map_element));
    if (elem != NULL) {
        elem->key = m->next_key;
    // If something bricks check if next_key overflows
        elem->value = v;
        list_push_back(&m->content, &elem->elem);  // Insert into the linked list. Elem = node, *prev, *next
        m->next_key++;
        
        return elem->key;
    };
    return -1;
}

value_t map_find(struct map* m, key_t k) {
    // Iterate over each element in the list.
    for (struct list_elem *e = list_begin(&m->content);
         e != list_end(&m->content);
         e = list_next(e)) {
         
        // Get the map_element struct that contains this list element.
        struct map_element *elem = list_entry(e, struct map_element, elem);
        
        // Check if this element's key matches the search key.
        if (elem->key == k) {
            return elem->value;
        }
    }
    
    // If no matching key is found, return NULL.
    return NULL;
}

value_t map_remove(struct map* m, key_t k){
    for (struct list_elem *e = list_begin(&m->content);
    e != list_end(&m->content);
    e = list_next(e)) {
    
   // Get the map_element struct that contains this list element.
   struct map_element *elem = list_entry(e, struct map_element, elem);
   
   // Check if this element's key matches the search key.
   if (elem->key == k) {
        list_remove(e);
       char* value_t = elem->value;
       free(elem);
       return value_t;

   }
}



    return NULL;
} 

void map_for_each(struct map* m,
    void (*exec)(key_t k, value_t v, int aux),
    int aux){
        
    for (struct list_elem *e = list_begin(&m->content);
    e != list_end(&m->content);
    e = list_next(e)) {
    
   // Get the map_element struct that contains this list element.
   struct map_element *elem = list_entry(e, struct map_element, elem);
   
   exec(elem->key,elem->value, aux); 
}
    } 

void map_remove_if(struct map* m,
    bool (*cond)(key_t k, value_t v, int aux),
    int aux){

    for (struct list_elem *e = list_begin(&m->content);
    e != list_end(&m->content);
    e = list_next(e)) {
    
   // Get the map_element struct that contains this list element.
   struct map_element *elem = list_entry(e, struct map_element, elem);
   
    if (cond(elem->key,elem->value, aux))
   {
    map_remove(m,elem->key);
   };  
}
    
} 
