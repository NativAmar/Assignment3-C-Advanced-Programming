#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MultiValueHashTable.h"
#include "HashTable.h"
#include "LinkedList.h"


struct MultiValueHashTable_s {
    hashTable table;
    CopyFunction copyVal;
    FreeFunction freeVal;
    PrintFunction printVal;
    PrintFunction printKey;
    EqualFunction equalKey;
};


MultiValueHashTable createMultiValueHashTable(CopyFunction copyKey, CopyFunction copyValue, FreeFunction freeKey, FreeFunction freeValue,
                                        PrintFunction printKey, PrintFunction printValue, EqualFunction equalKey, TransformIntoNumberFunction hashFunc, int hashSize) {

    if (hashSize <= 0) {
        return NULL;
    }

    MultiValueHashTable multiValueTbl = (MultiValueHashTable)malloc(sizeof(MultiValueHashTable));
    if (multiValueTbl == NULL) {
        return NULL;
    }

    multiValueTbl->table = createHashTable(copyKey, freeKey, printKey, copyValue, freeValue, printValue, equalKey, hashFunc, hashSize);
    if (multiValueTbl->table == NULL) {
        free(multiValueTbl);
        return NULL;
    }

    multiValueTbl->copyVal = copyValue;
    multiValueTbl->freeVal = freeValue;
    multiValueTbl->printVal = printValue;
    multiValueTbl->printKey = printKey;
    multiValueTbl->equalKey = equalKey;
    return multiValueTbl;
};


status destroyMultiValueHashTable(MultiValueHashTable multiValueHashTable) {
    if (multiValueHashTable == NULL) {
        return failure;
    }
    if (destroyHashTable(multiValueHashTable->table)) {
        return failure;
    }
    free(multiValueHashTable);
    return success;
};


status addToMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key, Element value) {
    if (multiValueHashTable == NULL || key == NULL || value == NULL) {
        return failure;
    }
    LinkedList valuesList = lookupInHashTable(multiValueHashTable->table, key);

    if (valuesList == NULL) {
        //first element for this key
        valuesList = createLinkedList(multiValueHashTable->copyVal, multiValueHashTable->freeVal, multiValueHashTable->equalKey);
        if (valuesList == NULL) {
            return failure;
        }
        if (addToHashTable(multiValueHashTable->table, key, valuesList) != success) {
            destroyList(valuesList);
            return failure;
        }
    }
    return appendNode(valuesList, value);
};


LinkedList lookupInMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key) {
    if (multiValueHashTable == NULL || key == NULL) {
        return NULL;
    }
    LinkedList valuesList = lookupInHashTable(multiValueHashTable->table, key);
    if (valuesList == NULL) {
        return NULL;
    }
    return valuesList;
}


status removeFromMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key, Element value) {
    if (multiValueHashTable == NULL || key == NULL || value == NULL) {
        return failure;
    }
    LinkedList valuesList = lookupInHashTable(multiValueHashTable->table, key);
    if (valuesList == NULL) {
        return failure;
    }
    return deleteNode(valuesList, value);
};


status displayMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key) {
    if (multiValueHashTable == NULL || key == NULL) {
        return failure;
    }
    LinkedList valuesList = lookupInHashTable(multiValueHashTable->table, key);
    if (valuesList == NULL) {
        return failure;
    }
    multiValueHashTable->printKey(key);
    displayList(valuesList, multiValueHashTable->printVal);
    return success;
}


