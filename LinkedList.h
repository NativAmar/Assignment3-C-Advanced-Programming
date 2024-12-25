#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "Defs.h"

typedef struct Linked_List* LinkedList;

LinkedList createLinkedList(CopyFunction copyFunc, FreeFunction freeFunc, EqualFunction equalFunc);

status destroyList(LinkedList list);

status appendNode(LinkedList list, Element element);

status deleteNode(LinkedList list, Element element);

//print the whole list by the insertion order --> first - last
void displayList(LinkedList list, PrintFunction printFunction);

//return the value stored in the node at this index or NULL if the index is illegal. *the nodes will be numbered from 1 to up
Element getDataByIndex(LinkedList list, int index);

int getLengthList(LinkedList list);

//The function checks if there is a node which consist a value that contains the same part that was received, and if so,
//returns the value stored in the node, if not, returns NULL. If there is more than one element suitable for the search - the first one will be returned.
Element searchByKeyInList(LinkedList list, Element key);



#endif //LINKED_LIST_H
