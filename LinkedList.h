#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "Defs.h"

typedef struct Linked_List* LinkedList;

//The createLinkedList function initializes a new linked list, sets its function pointers for element management (copy, free, comparison, and print), and returns the allocated list structure.
LinkedList createLinkedList(CopyFunction copyFunc, FreeFunction freeFunc, EqualFunction equalElemFunc, EqualFunction equalKeyFunc, PrintFunction printFunc);

//The destroyList function deallocates all nodes and their associated data in the linked list, then frees the list structure itself to release all memory.
status destroyList(LinkedList list);

//The appendNode function adds a new element to the end of the linked list by creating a copy of the element,
//allocating memory for a new node, and linking it to the last node in the list while updating the list size.
status appendNode(LinkedList list, Element element);

//The deleteNode function removes a specified element from the linked list by locating it using a comparison function, adjusting the node pointers,
//deallocating memory for the element and node, and updating the list size.
status deleteNode(LinkedList list, Element element);

//print the whole list by the insertion order --> first - last
status displayList(LinkedList list);

//return the value stored in the node at this index or NULL if the index is illegal. *the nodes will be numbered from 1 to up
Element getDataByIndex(LinkedList list, int index);

//The getLengthList function returns the number of elements in the linked list or 0 if the list is NULL.
int getLengthList(LinkedList list);

//The function checks if there is a node which consist a value that contains the same part that was received, and if so,
//returns the value stored in the node, if not, returns NULL. If there is more than one element suitable for the search - the first one will be returned.
Element searchByKeyInList(LinkedList list, Element key);


#endif //LINKED_LIST_H
