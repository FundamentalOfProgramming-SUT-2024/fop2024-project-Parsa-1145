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
void removeItemFromLinkedList(LinkedList* list, void* element);
void linkedListInsert(LinkedList* linkedList, void* element, int index);
void popLinkedList(LinkedList* linkedList);
void emptyLinkedList(LinkedList* list);

#endif