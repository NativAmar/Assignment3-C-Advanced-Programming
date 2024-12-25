#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H
#include "Defs.h"

typedef struct Key_Value_Pair* KeyValuePair;

KeyValuePair createKeyValuePair(Element key,Element value,CopyFunction copy_key_function, FreeFunction free_key_function,CopyFunction copy_value_function,
                                    FreeFunction free_value_function,PrintFunction print_key_function, PrintFunction print_value_function, EqualFunction equal_key_function);

status destroyKeyValuePair(KeyValuePair pair);

void displayValue(KeyValuePair pair);

void displayKey(KeyValuePair pair);

Element getValue(KeyValuePair pair);

Element getKey(KeyValuePair pair);

bool isEqualKey(KeyValuePair pair, Element key);

Element pairCopy(Element pair);//should check the input / output (maybe to solve the data types in other way)

status printPair(Element pair);//also here


#endif