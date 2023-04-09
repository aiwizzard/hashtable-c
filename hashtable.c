

#include "hashtable.h"
#include <string.h>
#include <stdio.h>

typedef struct entry {
    char *key;
    void *object;
    struct entry *next;
} entry;

struct _hash_table {
    uint32_t size;
    hashfunction *hash;
    entry **elements;
};


static size_t hash_table_index(hash_table *ht, const char *key) {
    size_t result = (ht->hash(key, strlen(key)) % ht->size);
    return result;
}


hash_table *hash_table_create(uint32_t size, hashfunction *hf) {
    hash_table *ht = malloc(sizeof(*ht));
    ht->size = size;
    ht->hash = hf;
    // Note that calloc zeros out the memory
    ht->elements = calloc(sizeof(entry*), ht->size);
    return ht;
}


void hash_table_destroy(hash_table *ht) {
     // Clean up individual elements
     for (uint32_t i = 0; i < ht->size; i++) {
         while (ht->elements[i]) {
             entry *tmp = ht->elements[i];
             ht->elements[i] = ht->elements[i]->next;
             free(tmp->key);
             free(tmp->object);
             free(tmp);
         }
     }

     free(ht->elements);
     free(ht);
}


void hash_table_print(hash_table *ht) {
    printf("Start Table\n");
    for (uint32_t i=0; i < ht->size; i++) {
        if (ht->elements[i] == NULL) {
            // do we want to print out null entries?
            // printf("\t%i\t---\n", i);
        } else {
            printf("\t%i\t\n", i);
            entry *tmp = ht->elements[i];
            while (tmp != NULL) {
                printf("\"%s\"(%p) - ", tmp->key, tmp->object);
                tmp = tmp->next;
            }
            printf("\n");
        }
    }
}


bool hash_table_insert(hash_table *ht, const char *key, void *obj) {
    if (key == NULL || obj == NULL || ht == NULL) return false;
    size_t index = hash_table_index(ht, key);

    if (hash_table_lookup(ht, key) != NULL) return false;

    // Create a new entry
    entry *e = malloc(sizeof(*e));
    e->object = obj;
    
    // e->key = malloc(strlen(key)+1);
    // strcpy(e->key, key);
    e->key = strdup(key);

    // Insert entry
    e->next = ht->elements[index];
    ht->elements[index] = e;
    return true;
}


void *hash_table_lookup(hash_table *ht, const char *key) {
    if (key == NULL || ht == NULL) return false;
    size_t index = hash_table_index(ht, key);

    entry *tmp = ht->elements[index];
    while(tmp != NULL && strcmp(tmp->key, key) !=0) {
        tmp = tmp->next;
    }
    if (tmp == NULL) return NULL;
    return tmp->object;
}


void *hash_table_delete(hash_table *ht, const char *key) {
    if (key == NULL || ht == NULL) return false;
    size_t index = hash_table_index(ht, key);

    entry *tmp = ht->elements[index];
    entry *prev = NULL;
    while(tmp != NULL && strcmp(tmp->key, key) !=0) {
        tmp = tmp->next;
    }
    if (tmp == NULL) return NULL;
    if (prev == NULL) {
        // Deleting the head of the list
        ht->elements[index] = tmp->next;
    } else {
        // Deleting from other than the head
        prev->next = tmp->next;
    }
    void *result = tmp->object;
    free(tmp);
    return result;
}
