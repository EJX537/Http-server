//-------------------------------------------------------------------------------------------------
// List.c
//-------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
// Structs ----------------------------------------------------------------------------------------
typedef struct NodeObj* Node;
typedef struct NodeObj {
    void* data;
    Node next;
    Node prev;
} NodeObj;
typedef struct ListObj {
    Node front;
    Node cursor;    
    Node back;
    int length;
    int index;
} ListObj;
// create New List
List newList(void) {
    List list;
    list = malloc(sizeof(ListObj));
    list->front = list->back = list->cursor = NULL;
    list->length = 0;
    list->index = -1;
    return list;
}
// Create New Node
Node newNode_List(void* x) {
    Node node;
    node = malloc(sizeof(NodeObj));
    node->data = x;
    node->next = NULL;
    node->prev = NULL;
    return node;
}
// Free List
void freeList(List* pList) {
    if (pList != NULL && *pList != NULL) {   
        clear_list(*pList);
        free(*pList);
        *pList = NULL;
    }
}
// Free Node
void freeNode_List(Node* pNode) {
    if (pNode != NULL && *pNode != NULL) {   
        free(*pNode);
        *pNode = NULL;
    }
}
// Return Length of pointer
int length(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling length() on NULL list reference\n");
        exit(EXIT_FAILURE);
    }
    return L->length;
}
// Return Index of pointer
int Index(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling index() on NULL list reference\n");
        exit(EXIT_FAILURE);        
    }
    if (L->cursor == NULL) {
        return -1;
    }
    return L->index;
}
// Return front index of list
void* front(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling front() on NULL list reference\n");
        exit(EXIT_FAILURE);
    }
    return L->front->data;
}
// Return last index of list
void* back(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling back() on NULL list reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length == 0) {
        printf("List Error: calling back() on empty list reference\n");
        exit(EXIT_FAILURE);
    }
    return L->back->data;
}
// Return data of the cursor 
void* get(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling get() on NULL list reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length < -1) {
        printf("List Error: calling get() on empty list reference\n");
        exit(EXIT_FAILURE);
    }
    if (Index(L) < 0) {
        printf("List Error: calling get() on out of bounds\n");
        exit(EXIT_FAILURE);
    }
    return L->cursor->data;
}
// Manipulation procedures ------------------------------------------------------------------------
void clear_list(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling clear() on NULL list reference\n");
        exit(EXIT_FAILURE);
    }
    int j = L->length;
    for (int i=0; i < j; i++) {
        deleteFront(L);
    }
    L->index = -1;
    L->front = L->back = L->cursor = NULL;
}
void set(List L, void* x) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling set() on NULL list reference\n");
        exit(EXIT_FAILURE);
    }
    if (Index(L) < 0) {
        printf("List Error: calling set() on out of bounds pointer reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length < 0) {
        printf("List Error: calling set() on empty List reference\n");
        exit(EXIT_FAILURE);
    }
    L->cursor->data = x;
}
// Move cursor to front Node
void moveFront(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling moveFront() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length < 1) {
        return;
    }
    L->index = 0;
    L->cursor = L->front;
}
// Move cursor to back Node
void moveBack(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling moveBack() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length < 0) {
        printf("List Error: calling moveBack() on empty List\n");
        return;
    }
    L->index = L->length - 1;
    L->cursor = L->back;
}
// Increment cursor back once
void movePrev(List L) {
    // Preconditions
    if (L->cursor == NULL) {
        printf("List Error: calling movePrev() on NULL List reference$\n");
        exit(EXIT_FAILURE);
    } else if (L->cursor == L->front) { // Moving out of bounds 
        L->cursor = NULL;
        L->index = -1;
    } else {
        L->cursor = L->cursor->prev;
        L->index--;
    }
}
// Increment cursor foward once
void moveNext(List L) {
    // Preconditions
    if (L->cursor == NULL) {
        return;
    } else if (L->cursor == L->back || L->index == L->length) { // Moving out of bounds
        L->cursor = NULL;
        L->index = -1;
    } else {
        L->cursor = L->cursor->next;
        L->index++;
    }
}
// Add to front of list
void prepend(List L, void* x) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling prepend() on empty List reference\n");
        exit(EXIT_FAILURE);
    } 
    Node node = newNode_List(x);
    if (L->length == 0) { // First node prepend
        L->front = L->back = node;
        L->length++;
    } else { // All other cases
        Node temp = L->front;
        L->front->prev = node;
        L->front = node;
        node->next = temp;
        L->length++;
        if (Index(L) >= 0) {
            L->index++;
        }
    }
}
// Add to end of list
void append(List L, void* x) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling append() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    Node node = newNode_List(x);
    if (L->length == 0) { // First node append
        L->front = L->back = node;
        L->length++;
    } else { // All other cases
        Node oldBack = L->back;
        oldBack->next = node;
        node->prev = oldBack;
        L->back = node;
        L->length++;
    }
}
// Insert after the cursor
void insertBefore(List L, void* x) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling insertBefore() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length < 0) {
        printf("List Error: calling insertBefore() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (Index(L) < 0) {
        printf("List Error: calling insertBefore() on pointer NULL reference\n");
        exit(EXIT_FAILURE);
    }
    Node node = newNode_List(x);
    Node temp = L->cursor;
    if (temp->prev == NULL) { // Add to the beginning of the list
        node->prev = NULL;
        L->front = node;
    } else {
        temp->prev->next = node;
        node->prev = temp->prev;
    }
    node->next = temp;
    temp->prev = node;
    L->length++;
    L->index++;
}
// Insert after the cursor
void insertAfter(List L, void* x) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling insertAfter() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length < 0) {
        printf("List Error: calling insertAfter() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (Index(L) < 0) {
        printf("List Error: calling insertAfter() on pointer NULL reference\n");
        exit(EXIT_FAILURE);
    }
    Node node = newNode_List(x);
    Node temp = L->cursor;
    if (temp->next == NULL) { // Add to end of list
        L->back = node;
        node->next = NULL;
    } else {
        temp->next->prev = node;
        node->next = temp->next;
    }
    node->prev = temp;
    temp->next = node;
    L->length++;
}
// Deleting first node
void deleteFront(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling deleteFront() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length < 0) {
        printf("List Error: calling deleteFront() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    Node newFront = L->front->next;
    Node oldFront = L->front;
    if (oldFront == L->cursor) { // If deleteFront while cursor is at front
        L->index = -1;
        L->cursor = NULL;
    } else { // Always index down if not at front
        L->index--;
    }
    if (L->length == 2) { // If deleteFront when there is two nodes
        L->back = newFront;
    }
    L->front = newFront;
    L->length--;
    freeNode_List(&oldFront);
}
// Deleting last node
void deleteBack(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling deleteBack() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (L->length < 0) {
        printf("List Error: calling deleteBack() on empty List reference\n");;
        exit(EXIT_FAILURE);
    }
    Node newBack = L->back->prev;
    Node oldBack = L->back;
    if (oldBack == L->cursor) { // If deleteBack while at end
        L->index = -1;
        L->cursor = NULL;
    }
    if (L->length == 2) { // If deleteBack when there is two nodes
        L->front = newBack;
    } 
    L->back = newBack;
    L->length--;
    freeNode_List(&oldBack);
}
// Deleting cursor node
void delete(List L) {
    // Preconditions
    if (L == NULL) {
        printf("List Error: calling delete() on NULL List reference");
        exit(EXIT_FAILURE);
    }
    if (Index(L) < 0) {
        printf("List Error: calling delete() on out of bounds pointer reference\n");;
        exit(EXIT_FAILURE);
    }
    if (L->length < 0) {
        printf("List Error: calling delete() on empty() list reference\n");
        exit(EXIT_FAILURE);
    }
    Node current = L->cursor;
    Node before = L->cursor->prev;
    Node after = L->cursor->next;
    // Deleting the first node
    if (Index(L) == 0) { // deleting the first node
        L->front = current->next;
    } 
    // Deleting the last node
    else if (Index(L) == L->length - 1) { // deleting the last node
        L->back = current->prev;
    } else { 
        before->next = after;
        after->prev = before;
    }
    L->index = -1;
    L->length--; 
    freeNode_List(&current);
}
// Other operations -------------------------------------------------------------------------------
// Print the list
void printList(FILE *out, List L) {
    Node temp = L->front;
    for (int i=0; i < L->length; i++) {
        fprintf(out, "%p ", temp->data);
        temp = temp->next;
    }
}
