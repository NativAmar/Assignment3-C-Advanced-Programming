#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H
#include "Defs.h"

typedef struct Key_Value_Pair* KeyValuePair;

//The createKeyValuePair function initializes a key-value pair by copying the provided key and value,
//assigning function pointers for memory management, printing, and comparison, and returning the allocated structure.
KeyValuePair createKeyValuePair(Element key,Element value,CopyFunction copy_key_function, FreeFunction free_key_function,CopyFunction copy_value_function,
                                    FreeFunction free_value_function,PrintFunction print_key_function, PrintFunction print_value_function, EqualFunction equal_key_function);

//The destroyKeyValuePair function deallocates the memory for the key and value using their respective free functions, then frees the key-value pair structure itself.
status destroyKeyValuePair(KeyValuePair pair);

//The displayValue function prints the value of the given key-value pair using the print_value_function function, if the pair is not NULL.
void displayValue(KeyValuePair pair);

//The displayKey function prints the key of the given key-value pair using the print_key_function function, if the pair is not NULL.
void displayKey(KeyValuePair pair);

//The getValue function returns the value of the given key-value pair, or NULL if the pair is NULL.
Element getValue(KeyValuePair pair);

//The getKey function returns the key from a key-value pair, returning NULL if the pair is NULL.
Element getKey(KeyValuePair pair);

//The isEqualKey function checks if the key in a key-value pair is equal to a given key using the pair's equalKey function.
bool isEqualKey(KeyValuePair pair, Element key);


#endif