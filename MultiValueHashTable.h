#ifndef MULTI_VALUE_HASH_TABLE_H
#define MULTI_VALUE_HASH_TABLE_H
#include "Defs.h"
#include "LinkedList.h"


typedef struct MultiValueHashTable_s * MultiValueHashTable;

//The createMultiValueHashTable function initializes and allocates memory for a multi-value hash table, where each key can have multiple associated values.
MultiValueHashTable createMultiValueHashTable(CopyFunction copyKey, CopyFunction copyValue, FreeFunction freeKey, FreeFunction freeValue, PrintFunction printKey,
                                                PrintFunction printValue, EqualFunction equalKey, EqualFunction equalValue, TransformIntoNumberFunction hashFunc, int hashSize);

//The destroyMultiValueHashTable function releases all allocated memory for a multi-value hash table
status destroyMultiValueHashTable(MultiValueHashTable multiValueHashTable);

//The addToMultiValueHashTable function adds a value to the list of values associated with a key in the multi-value hash table, ensuring no duplicate values for the same key.
status addToMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key, Element value);

//The lookupInMultiValueHashTable function retrieves the linked list of values associated with a given key in the multi-value hash table, or returns NULL if the key is not found.
LinkedList lookupInMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key); //return the whole list of the matches values

//The removeFromMultiValueHashTable function removes a specific value associated with a given key in a multi-value hash table, and deletes the key from the table if its value list becomes empty.
status removeFromMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key, Element value);

//The displayMultiValueHashTable function prints the specified key and its associated values in a multi-value hash table.
status displayMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key);

#endif
