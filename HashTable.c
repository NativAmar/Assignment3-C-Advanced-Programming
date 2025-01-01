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

//I wrote the documentation in the C file because we are not allowed to edit the header file :)


//The printPair function prints the key and value of a key-value pair, using the pair's respective display functions, and returns success if both key and value are valid.
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
        return success;
    }
    return failure;
}


//The pairCopy function casts the given element to a KeyValuePair and returns it, or returns NULL if the element is NULL.
Element pairCopy(Element pair) {
    //Input validation
    KeyValuePair pairCopy = (KeyValuePair)pair;
    if (pairCopy == NULL) {
        return NULL;
    }
    return pairCopy;
}


//The equalKeyValuePair function checks if the keys of two key-value pairs are equal by comparing their key elements.
bool equalKeyValuePair(Element key1, Element key2) {
    //Input validation
    if (key1 == NULL || key2 == NULL) {
        return false;
    }
    KeyValuePair kvp1 = (KeyValuePair)key1;
    KeyValuePair kvp2 = (KeyValuePair)key2;
    return getKey(kvp1) == getKey(kvp2);
}

//The isEqualKey_Hash function checks if two keys are equal by calling the isEqualKey function, returning false if either key is NULL.
bool isEqualKey_Hash(Element key1, Element key2) {
    //Input validation
    if (key1 == NULL || key2 == NULL) {
        return false;
    }
    return isEqualKey(key1, key2);
}


//The createHashTable function initializes and allocates memory for a hash table with the given functions for key and value handling,
//and returns a pointer to the created hash table, or NULL if any allocation fails.
hashTable createHashTable(CopyFunction copyKey, FreeFunction freeKey, PrintFunction printKey, CopyFunction copyValue, FreeFunction freeValue,
                            PrintFunction printValue, EqualFunction equalKey, TransformIntoNumberFunction transformIntoNumber, int hashNumber)
{
    //Input validation
    if (hashNumber < 1 || !copyKey || !freeKey || !printKey || !copyValue || !freeValue || !printValue || !equalKey || !transformIntoNumber) {
        return NULL;
    }
    //Allocate memory for the structure
    hashTable hash_Table = (hashTable)malloc(sizeof(struct hashTable_s));
    //allocation check
    if (hash_Table == NULL) {
        return NULL;
    }
    // Allocate memory for the array of linked lists
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
}


//The destroyHashTable function frees the memory allocated for the hash table, including the linked lists stored in the table if exist,
//and returns success or failure depending on whether the destruction of the lists succeeds.
status destroyHashTable(hashTable hash_Table) {
    //Input validation
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
}


//The addToHashTable function attempts to insert a key-value pair into the hash table, ensuring that the key does not already exist in the table,
//creating necessary linked lists at the appropriate hash table index if they do not exist, and handling all associated memory management and error handling.
status addToHashTable(hashTable hash_Table, Element key, Element value) {
    //Input validation
    if (hash_Table == NULL || key == NULL || value == NULL ) {
        return argumentFailure;
    }
    if (lookupInHashTable(hash_Table, key) != NULL) { //key already exist
        return failure;
    }

    int indexToStore = hash_Table->hashFunction(key) % hash_Table->size; //hash
    LinkedList list = hash_Table->lists_table[indexToStore];

    if (list == NULL) {
        //first pair for this index
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
}


//The lookupInHashTable function searches for a key in the hash table and returns the associated value if found, or NULL if the key does not exist or if any input is invalid.
Element lookupInHashTable(hashTable hash_Table, Element key) {
    //Input validation
    if (hash_Table == NULL || key == NULL) {
        return NULL;
    }
    int indexToLookup = hash_Table->hashFunction(key) % hash_Table->size;
    return getValue(searchByKeyInList(hash_Table->lists_table[indexToLookup], key));
}


//The removeFromHashTable function removes a key-value pair from the hash table using the specified key.
status removeFromHashTable(hashTable hash_Table, Element key) {
    //Input validation
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
}


//The displayHashElements function displays all elements in the hash table by iterating over each linked list at each index.
status displayHashElements(hashTable hash_Table) {
    //Input validation
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
}



