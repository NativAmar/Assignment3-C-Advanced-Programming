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


status printPair(Element pair) {
    KeyValuePair pairCopy = (KeyValuePair)pair;
    if (pairCopy == NULL) {
        return failure;
    }
    Element key = getKey(pairCopy);
    Element value = getValue(pairCopy);

    if (key!= NULL && value != NULL) {
        displayKey(pairCopy);  // Print the key using the provided function
        printf(" ");
        displayValue(pairCopy);  // Print the value using the provided function
        //printf("\n");  // End the line
        return success;
    }
    return failure;
}


Element pairCopy(Element pair) {
    KeyValuePair pairCopy = (KeyValuePair)pair;
    if (pairCopy == NULL) {
        return NULL;
    }
    return pairCopy;
}


bool equalKeyValuePair(Element key1, Element key2) {
    if (key1 == NULL || key2 == NULL) {
        return false;
    }
    KeyValuePair kvp1 = (KeyValuePair)key1;
    KeyValuePair kvp2 = (KeyValuePair)key2;
    return getKey(kvp1) == getKey(kvp2);
}

bool isEqualKey_Hash(Element key1, Element key2) {
    if (key1 == NULL || key2 == NULL) {
        return false;
    }
    KeyValuePair kvp1 = (KeyValuePair)key1;
    return isEqualKey(key1, key2);
}


hashTable createHashTable(CopyFunction copyKey, FreeFunction freeKey, PrintFunction printKey, CopyFunction copyValue, FreeFunction freeValue,
                            PrintFunction printValue, EqualFunction equalKey, TransformIntoNumberFunction transformIntoNumber, int hashNumber)
{
    if (hashNumber < 1) {
        return NULL;
    }
    hashTable hash_Table = (hashTable)malloc(sizeof(struct hashTable_s));
    //allocation check
    if (hash_Table == NULL) {
        return NULL;
    }

    hash_Table->lists_table = (LinkedList*)calloc(hashNumber,sizeof(LinkedList));
    //allocation check
    if (hash_Table->lists_table == NULL) {
        free(hash_Table);
        return NULL;
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
        return argumentFailure;
    }
    for (int i = 0; i < hash_Table->size; i++) {
        if (hash_Table->lists_table[i] != NULL) {
            if (destroyList(hash_Table->lists_table[i]) != success) {
                return failure;
            }
        }
    }
    free(hash_Table->lists_table);
    free(hash_Table);
    return success;
};


//LinkedList Copy function issue
status addToHashTable(hashTable hash_Table, Element key, Element value) {
    if (hash_Table == NULL || key == NULL || value == NULL ) {
        return argumentFailure;
    }
    if (lookupInHashTable(hash_Table, key) != NULL) { //key already exist
        return failure;
    }

    int indexToStore = hash_Table->hashFunction(key) % hash_Table->size;
    LinkedList list = hash_Table->lists_table[indexToStore];

    if (list == NULL) {
        list = createLinkedList(pairCopy, (Element)destroyKeyValuePair, equalKeyValuePair, isEqualKey_Hash, printPair);
        if (list == NULL) {
            return memoryFailure;
        }
        hash_Table->lists_table[indexToStore] = list;
    }

    KeyValuePair pair = createKeyValuePair(key, value, hash_Table->copyKey, hash_Table->freeKey, hash_Table->copyValue,
                            hash_Table->freeValue,hash_Table->printKey, hash_Table->printValue, hash_Table->equalKey);

    if (pair == NULL) {
        return memoryFailure;
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
    return getValue(searchByKeyInList(hash_Table->lists_table[indexToLookup], key));
};


status removeFromHashTable(hashTable hash_Table, Element key) {
    if (hash_Table == NULL || key == NULL) {
        return argumentFailure;
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
        return argumentFailure;
    }
    for (int i = 0; i < hash_Table->size; i++) {
        LinkedList list = hash_Table->lists_table[i];
        if (list != NULL) {
            displayList(list);
        }
    }
    return success;
};



