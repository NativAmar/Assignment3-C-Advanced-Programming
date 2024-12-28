#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"
#include "Defs.h"


typedef struct node_s{
        Element data;
        struct node_s *next;
} node;

struct Linked_List {
        node *head;
        int size; //initialized with no nodes with value of 0
        CopyFunction copy;
        FreeFunction free;
        PrintFunction print;
        EqualFunction equalElem;
        EqualFunction equalKey;
};

LinkedList createLinkedList(CopyFunction copyFunc, FreeFunction freeFunc, EqualFunction equalElemFunc,EqualFunction equalKeyFunc , PrintFunction printFunc ) {
        if (copyFunc == NULL || freeFunc == NULL || equalElemFunc == NULL || printFunc == NULL) {
                return NULL;
        }
        LinkedList list = (LinkedList)malloc(sizeof(struct Linked_List));
        //allocation check
        if (list == NULL) {
                return NULL;
        }
        list->head = NULL;
        list->size = 0;
        list->copy = copyFunc;
        list->free = freeFunc;
        list->equalElem = equalElemFunc;
        list->equalKey = equalKeyFunc;
        list->print = printFunc;
        return list;
};

status destroyList(LinkedList list) {
        if (list == NULL) {
                return argumentFailure;
        }
        if (list->head == NULL || list->size == 0) {
                free(list);
                return success;
        }
        node *curr = list->head;
        while (curr != NULL) {
                node *next = curr->next;
                list->free(curr->data);
                free(curr);
                curr = next;
        }
        free(list);
        return success;
};

status appendNode(LinkedList list, Element element) {
        //input check
        if (list == NULL || element == NULL) {
                return argumentFailure;
        }
        node* newNode = (node*)malloc(sizeof(node));
        //allocation check
        if (newNode == NULL) {
                return memoryFailure;
        }
        newNode->data = list->copy(element);
        newNode->next = NULL;

        if (list->head == NULL) {
                list->head = newNode;
        }
        else {
                //reach the last node at the list
                node* curr = list->head;
                while (curr->next != NULL) {
                        curr = curr->next;
                }
                curr->next = newNode;
        }
        list->size++;
        return success;
};


status deleteNode(LinkedList list, Element element) {
        if (list == NULL || element == NULL) {
                return argumentFailure;
        }
        node* curr = list->head;
        node* prev = NULL;
        while (curr != NULL) {
                if (list->equalElem(curr->data, element)) {
                        if (prev == NULL) {
                                list->head = curr->next;
                        }
                        else {
                                prev->next = curr->next;
                        }
                        list->free(curr->data);
                        free(curr);
                        list->size--;
                        return success;
                }
                prev = curr;
                curr = curr->next;
        }
        return failure;
};


status displayList(LinkedList list) {
        if (list == NULL) {
                return argumentFailure;
        }
        node* curr = list->head;
        while (curr != NULL) {
                list->print(curr->data);
                curr = curr->next;
        }
        return success;
};


Element getDataByIndex(LinkedList list, int index) {
        if (list == NULL || index < 0 || index > list->size) {
                return NULL;
        }
        node* curr = list->head;
        for (int i = 1; i < index; i++) {
                curr = curr->next;
        }
        return curr->data;
};


int getLengthList(LinkedList list) {
        if (list == NULL) {
                return 0;
        }
        return list->size;
};


//I should ask myself if it's better to get the equalFunc here or at the createList
Element searchByKeyInList(LinkedList list, Element key) {
        if (list == NULL || key == NULL) {
                return NULL;
        }
        node* curr = list->head;
        while (curr != NULL) {
                if (list->equalKey(curr->data, key)) {
                        return list->copy(curr->data);
                }
                curr = curr->next;
        }
        return NULL;
};

