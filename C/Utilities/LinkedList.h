#ifndef LINKEDLIST
#define LINKEDLIST

typedef struct{
    void** data;

    int dataSize;
    int size;

}LinkedList;

void createLinkedList(LinkedList* list, int dataSize);
void** linkedListGetElement(LinkedList* list, int index);
void linkedListDeleteElement(LinkedList* list, int index);
void linkedListPushBack(LinkedList* list, void* elment);
//void insetInLinkedList(LinkedList* linkedList, void* element, int index);
void popLinkedList(LinkedList* linkedList);

#endif