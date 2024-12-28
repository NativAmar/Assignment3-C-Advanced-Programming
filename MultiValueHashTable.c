#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MultiValueHashTable.h"
#include "HashTable.h"
#include "KeyValuePair.h"
#include "LinkedList.h"


struct MultiValueHashTable_s {
    hashTable table;
    CopyFunction copyVal;
    FreeFunction freeVal;
    PrintFunction printVal;
    PrintFunction printKey;
    EqualFunction equalValue;
};


Element copyElement_mvht(Element element) {
    KeyValuePair pair = (KeyValuePair)element;
    return pair;
}

Element copyLinkedList(Element element) {
    return element;
}

status destroyLinkedList(Element element) {
    return destroyList(element);
}

status displayLinkedList(Element element) {
    LinkedList list = (LinkedList)element;
    return displayList(list);
}

status freeElement_mvht(Element element) {
    KeyValuePair pair = (KeyValuePair)element;
    Element key = getKey(pair);
    LinkedList values = getValue(pair);
    destroyList(values);
    destroyKeyValuePair(pair);
}

status printElement_mvht(Element element) {
    KeyValuePair pair = (KeyValuePair)element;
    displayKey(pair);
    displayValue(pair);
    return success;
}


MultiValueHashTable createMultiValueHashTable(CopyFunction copyKey, CopyFunction copyValue, FreeFunction freeKey, FreeFunction freeValue,
                                        PrintFunction printKey, PrintFunction printValue, EqualFunction equalKey, EqualFunction equalValue, TransformIntoNumberFunction hashFunc, int hashSize) {

    if (hashSize <= 0) {
        return NULL;
    }

    MultiValueHashTable multiValueTbl = (MultiValueHashTable)malloc(sizeof(struct MultiValueHashTable_s));
    if (multiValueTbl == NULL) {
        return NULL;
    }

    multiValueTbl->table = createHashTable(copyKey, freeKey, printKey, copyLinkedList, destroyLinkedList, displayLinkedList, equalKey, hashFunc, hashSize);
    if (multiValueTbl->table == NULL) {
        free(multiValueTbl);
        return NULL;
    }

    multiValueTbl->copyVal = copyValue;
    multiValueTbl->freeVal = freeValue;
    multiValueTbl->printVal = printValue;
    //multiValueTbl->printVal = displayList;?
    multiValueTbl->printKey = printKey;
    multiValueTbl->equalValue = equalValue;
    return multiValueTbl;
};


status destroyMultiValueHashTable(MultiValueHashTable multiValueHashTable) {
    if (multiValueHashTable == NULL) {
        return argumentFailure;
    }
    if (destroyHashTable(multiValueHashTable->table) != success) {
        return failure;
    }
    free(multiValueHashTable);
    return success;
};


status addToMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key, Element value) {
    if (multiValueHashTable == NULL || key == NULL || value == NULL) {
        return argumentFailure;
    }
    LinkedList valuesList = lookupInHashTable(multiValueHashTable->table, key);

    if (valuesList == NULL) {
        //first element for this key
        valuesList = createLinkedList(multiValueHashTable->copyVal, multiValueHashTable->freeVal, multiValueHashTable->equalValue, multiValueHashTable->equalValue, multiValueHashTable->printVal);
        if (valuesList == NULL) {
            return memoryFailure;
        }
        if (appendNode(valuesList, value) != success) {
            return failure;
        }
        if (addToHashTable(multiValueHashTable->table, key, valuesList) != success) {
            destroyList(valuesList);
            return failure;
        }
        return success;
    }
    for (int i=0; i<getLengthList(valuesList); i++) {
        if (multiValueHashTable->equalValue(getDataByIndex(valuesList, i), value) == true) {
            return failure;
        }
    }
    return appendNode(valuesList, value);
};


LinkedList lookupInMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key) {
    if (multiValueHashTable == NULL || key == NULL) {
        return NULL;
    }
    return lookupInHashTable(multiValueHashTable->table, key);
}


status removeFromMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key, Element value) {
    if (multiValueHashTable == NULL || key == NULL || value == NULL) {
        return argumentFailure;
    }
    LinkedList valuesList = lookupInMultiValueHashTable(multiValueHashTable, key);
    if (valuesList == NULL) {
        return failure;
    }
    if (deleteNode(valuesList, value)  != success) {
        return failure;
    }

    if (getLengthList(valuesList) == 0) {
        return removeFromHashTable(multiValueHashTable->table, key);
    }
    return success;
};


status displayMultiValueHashTable(MultiValueHashTable multiValueHashTable, Element key) {
    if (multiValueHashTable == NULL || key == NULL) {
        return argumentFailure;
    }
    Element valuesList = lookupInMultiValueHashTable(multiValueHashTable, key);
    if (valuesList == NULL) {
        return failure;
    }
    multiValueHashTable->printKey(key);
    displayList(valuesList);
    return success;
}


