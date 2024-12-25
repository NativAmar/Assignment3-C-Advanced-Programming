#ifndef MULTI_VALUE_HASH_TABLE_H
#define MULTI_VALUE_HASH_TABLE_H
#include "Defs.h"
#include "LinkedList.h"


typedef struct MultiValueHashTable_s * MultiValueHashTable;//Multi hash table -> list of linked list pointers which every node at the list (in every list) consist key and another linked list for the matches values

MultiValueHashTable createMultiValueHashTable(CopyFunction copyKey, CopyFunction copyValue, FreeFunction freeKey, FreeFunction freeValue, PrintFunction printKey,
                                                PrintFunction printValue, EqualFunction equalKey, TransformIntoNumberFunction hashFunc, int hashSize);

status destroyMultiValueHashTable(MultiValueHashTable multiValueHashTable);

status addToMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key, Element value);

LinkedList lookupInMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key); //return the whole list of the matches values

status removeFromMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key, Element value);

status displayMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key);

#endif
