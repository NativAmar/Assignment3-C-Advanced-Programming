#include <stdio.h>
#include <stdlib.h>
#include "KeyValuePair.h"
#include "Defs.h"


struct Key_Value_Pair {
    Element Key;
    Element Value;
    CopyFunction copyKey;
    CopyFunction copyValue;
    FreeFunction freeKey;
    FreeFunction freeVal;
    PrintFunction printKey;
    PrintFunction printVal;
    EqualFunction equalKey;
};

KeyValuePair createKeyValuePair(Element key,Element value,CopyFunction copy_key_function, FreeFunction free_key_function, CopyFunction copy_value_function,
                                FreeFunction free_value_function, PrintFunction print_key_function,PrintFunction print_value_function, EqualFunction equal_key_function) {
    if (key == NULL || value == NULL || copy_key_function == NULL || free_key_function == NULL || free_value_function == NULL || copy_value_function ||
        print_key_function == NULL || print_value_function == NULL|| equal_key_function == NULL) {
        return NULL;
    }
    KeyValuePair pair = (KeyValuePair)malloc(sizeof(KeyValuePair));
    if (pair == NULL) {
        return NULL;
    }
    pair->copyKey = copy_key_function;
    pair->copyValue = copy_value_function;
    pair->Key = pair->copyKey(key);
    pair->Value = pair->copyValue(value);
    pair->freeKey = free_key_function;
    pair->freeVal = free_value_function;
    pair->printKey = print_key_function;
    pair->printVal = print_value_function;
    pair->equalKey = equal_key_function;
    return pair;
}


status destroyKeyValuePair(KeyValuePair pair) {
    if (pair == NULL) {
        return failure;
    }
    pair->freeKey(pair->Key);
    pair->freeVal(pair->Value);
    free(pair);
    return success;
}


void displayValue(KeyValuePair pair) {
    if (pair == NULL) {
        return;
    }
    pair->printVal(pair->Value);
}


void displayKey(KeyValuePair pair) {
    if (pair == NULL) {
        return;
    }
    pair->printKey(pair->Key);
}


Element getValue(KeyValuePair pair) {
    if (pair == NULL) {
        return NULL;
    }
    return pair->Value;
}


Element getKey(KeyValuePair pair) {
    if (pair == NULL) {
        return NULL;
    }
    return pair->Key;
}


bool isEqualKey(KeyValuePair pair, Element key) {
    if (pair == NULL || key == NULL) {
        return false;
    }
    if (pair->equalKey(pair->Key, key)) {
        return true;
    }
    return false;
}


Element pairCopy(Element pair) {
    KeyValuePair pairCopy = (KeyValuePair)pair;
    if (pairCopy == NULL) {
        return NULL;
    }
    return pairCopy;
}


status printPair(Element pair) {
    KeyValuePair pairCopy = (KeyValuePair)pair;
    if (pairCopy == NULL) {
        return failure;
    }
    Element key = getKey(pairCopy);
    Element value = getValue(pairCopy);

    if (key!= NULL && value != NULL) {
        pairCopy->printKey(key);  // Print the key using the provided function
        printf(" ");
        pairCopy->printVal(value);  // Print the value using the provided function
        printf("\n");  // End the line
        return success;
    }
    return failure;
}