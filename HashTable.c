#include <stdio.h>
#include <stdlib.h>
#include "HashTable.h"
#include "LinkedList.h"
#include "KeyValuePair.h"

struct hashTable_s {
    LinkedList* lists_table;
    int size;
    CopyFunction copyKey;
    CopyFunction copyValue;
    FreeFunction freeKey;
    FreeFunction freeValue;
    PrintFunction printKey;
    PrintFunction printValue;
    EqualFunction equalKey;
    TransformIntoNumberFunction hashFunction;
};



hashTable createHashTable(CopyFunction copyKey, FreeFunction freeKey, PrintFunction printKey, CopyFunction copyValue, FreeFunction freeValue,
                            PrintFunction printValue, EqualFunction equalKey, TransformIntoNumberFunction transformIntoNumber, int hashNumber)
{
    if (hashNumber < 1) {
        return NULL;
    }
    hashTable hash_Table = (hashTable)malloc(sizeof(hashTable));
    //allocation check
    if (hash_Table == NULL) {
        return NULL;
    }

    hash_Table->lists_table = (LinkedList*)malloc(sizeof(LinkedList) * hashNumber);
    //allocation check
    if (hash_Table->lists_table == NULL) {
        free(hash_Table);
        return NULL;
    }

    for (int i = 0; i < hashNumber; i++) {
        hash_Table->lists_table[i] = createLinkedList(pairCopy, (Element)destroyKeyValuePair, equalKey);
        //allocate check
        if (hash_Table->lists_table[i] == NULL) {
            //free memory which already allocated
            for (int j = 0; j < i; j++) {
                destroyList(hash_Table->lists_table[j]);
            }
            free(hash_Table->lists_table);
            free(hash_Table);
            return NULL;
        }
    }
    hash_Table->copyKey = copyKey;
    hash_Table->copyValue = copyValue;
    hash_Table->freeKey = freeKey;
    hash_Table->freeValue = freeValue;
    hash_Table->printKey = printKey;
    hash_Table->printValue = printValue;
    hash_Table->equalKey = equalKey;
    hash_Table->hashFunction = transformIntoNumber;
    hash_Table->size = hashNumber;
    return hash_Table;
};


status destroyHashTable(hashTable hash_Table) {
    if (hash_Table == NULL) {
        return failure;
    }
    for (int i = 0; i < hash_Table->size; i++) {
        if (destroyList(hash_Table->lists_table[i]) == failure) {
            return failure;
        }
    }
    free(hash_Table->lists_table);
    free(hash_Table);
    return success;
};


//LinkedList Copy function issue
status addToHashTable(hashTable hash_Table, Element key, Element value) {
    if (hash_Table == NULL || key == NULL || value == NULL ) {
        return failure;
    }
    int indexToStore = hash_Table->hashFunction(key) % hash_Table->size;
    LinkedList list = hash_Table->lists_table[indexToStore];

    if (searchByKeyInList(list, key) != NULL) {
        return failure;
    }

    KeyValuePair pair = createKeyValuePair(key, value, hash_Table->copyKey, hash_Table->freeKey, hash_Table->copyValue,
                            hash_Table->freeValue,hash_Table->printKey, hash_Table->printValue, hash_Table->equalKey);

    if (pair == NULL) {
        return failure;
    }

    if (appendNode(list, pair) == success) {
        return success;
    }

    destroyKeyValuePair(pair);
    return failure;
};


Element lookupInHashTable(hashTable hash_Table, Element key) {
    if (hash_Table == NULL || key == NULL) {
        return NULL;
    }

    int indexToLookup = hash_Table->hashFunction(key) % hash_Table->size;
    LinkedList list = hash_Table->lists_table[indexToLookup];

    KeyValuePair pair = searchByKeyInList(list, key);

    if (pair == NULL) {
        return NULL;
    }

    return getValue(pair);
};


status removeFromHashTable(hashTable hash_Table, Element key) {
    if (hash_Table == NULL || key == NULL) {
        return failure;
    }

    int indexToRemove = hash_Table->hashFunction(key) % hash_Table->size;
    LinkedList list = hash_Table->lists_table[indexToRemove];

    KeyValuePair pair = searchByKeyInList(list, key);
    if (pair == NULL) {
        return failure;
    }

    return deleteNode(list, pair);
};


status displayHashElements(hashTable hash_Table) {
    if (hash_Table == NULL) {
        return failure;
    }
    for (int i = 0; i < hash_Table->size; i++) {
        LinkedList list = hash_Table->lists_table[i];
        if (list != NULL) {
            displayList(list, printPair);
        }
    }
    return success;
};

