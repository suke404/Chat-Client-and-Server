#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include "list.h"


static Node node_list[LIST_MAX_NUM_NODES];
static List head_list[LIST_MAX_NUM_HEADS];
static List *head_stack_top = NULL;
static Node *node_stack_top = NULL;
static int initialization = 0;

// Removing nodes from the stack when it is in use
void pop_node()
{
    assert(node_stack_top != NULL);
    Node* temp = node_stack_top;

    if (node_stack_top->next != NULL)
    {
        node_stack_top = node_stack_top->next;
    }
    else
    {
        node_stack_top = NULL;
    }

    temp -> next = NULL;
}

// Removing list_head from the stack whe it is in use
void pop_list()
{
    assert(head_stack_top != NULL);
    Node* temp = head_stack_top;
    if (head_stack_top->next != NULL)
    {
        head_stack_top = head_stack_top->next;
    }
    else
    {
        head_stack_top = NULL;
    }
    temp -> next = NULL;
}

// Pushing freed nodes back to the stack
void push_node(Node *node)
{
    node->next = node_stack_top;
    node_stack_top = node;

    // re-init node
    node_stack_top -> pre = NULL;
}

// Pushing freed list_head back to the stack
void push_list(List *list)
{
    list->next = head_stack_top;
    head_stack_top = list;

    //re-init list
    head_stack_top -> current_node = NULL;
    head_stack_top -> head = NULL;
    head_stack_top -> tail = NULL;
    head_stack_top -> status = LIST_OOB_START;
    head_stack_top -> node_num = 0;
}

// Makes a new, empty list, and returns its reference on success.
// Returns a NULL pointer on failure.
List *List_create()
{
    // First time initilization
    if (initialization == 0)
    {
        // push all the avaliable node and list into two seperate stack
        for (int i = 0; i < LIST_MAX_NUM_HEADS; i++)
        {
            if (i == 0)
            {
                head_stack_top = &head_list[0]; 
            }
            else
            {
                List *temp = head_stack_top;
                head_stack_top = &head_list[i];
                head_stack_top->next = temp;
            }
        }
        for (int i = 0; i < LIST_MAX_NUM_NODES; i++)
        {
            if (i == 0)
            {
                node_stack_top = &node_list[0];
            }
            else
            {
                Node *temp = node_stack_top;
                node_stack_top = &node_list[i];
                node_stack_top->next = temp;
            }
        }
        initialization = 1;
    }

    if (head_stack_top != NULL)
    {
        List *temp = head_stack_top; // temp -> node
        pop_list();                  // pop,  head_stacktop = hst -> next
        temp->status = LIST_OOB_START;
        temp->head = NULL;
        temp->tail = NULL;
        temp->current_node = NULL;
        temp->node_num = 0;

        return temp;
    }
    else
    {
        return NULL;
    }
}

// Returns the number of items in pList.
int List_count(List *pList)
{
    return pList->node_num;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void *List_first(List *pList)
{
    assert(pList != NULL);
    if (pList->node_num != 0)
    {
        pList->current_node = pList->head;
        pList -> status = LIST_OOB_INSIDE;
        return pList -> current_node -> item;
    }
    else
    {
        pList -> current_node = NULL;
        pList -> status = LIST_OOB_START;
        return NULL;
    }
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void *List_last(List *pList)
{
    assert(pList != NULL);
    if (pList->node_num != 0)
    {// if it is not empty
        pList->current_node = pList->tail;
        return pList -> current_node -> item;
    }
    else
    {// if it is empty
        pList->current_node = NULL;
        return NULL;
    }
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer
// is returned and the current item is set to be beyond end of pList.
void *List_next(List *pList)
{
    assert(pList != NULL);
    if (pList->status == LIST_OOB_END)
    {
        return NULL;
    }
    if(pList->status == LIST_OOB_START && pList -> node_num == 0){
        pList -> status = LIST_OOB_END;
        return NULL;
    }

    if(pList->status == LIST_OOB_START && pList -> node_num > 0) {
        pList -> current_node = pList -> head;
        pList -> status = LIST_OOB_INSIDE;
        return pList -> current_node -> item;
    }

    if (pList->current_node->next != NULL)
    {
        pList->current_node = pList->current_node->next;
        pList -> status = LIST_OOB_INSIDE;
        return pList -> current_node -> item;
    }
    else
    {
        pList->status = LIST_OOB_END;
        pList->current_node = NULL;
        return NULL;
    }
}

// Backs up pList's current item by one, and returns a pointer to the new current item.
// If this operation backs up the current item beyond the start of the pList, a NULL pointer
// is returned and the current item is set to be before the start of pList.
void *List_prev(List *pList)
{
    assert(pList != NULL);
    if (pList->status == LIST_OOB_START)
    {
        return NULL;
    }
    
    if(pList->status == LIST_OOB_END && pList -> node_num == 0){
        pList -> status = LIST_OOB_START;
        return NULL;
    }

    if(pList->status == LIST_OOB_END && pList -> node_num > 0){
        pList -> current_node = pList -> tail;
        pList -> status = LIST_OOB_INSIDE;
        return pList -> current_node -> item;
    }

    if (pList->current_node->pre != NULL)
    {
        pList->current_node = pList->current_node->pre;
        pList -> status = LIST_OOB_INSIDE;
        return pList->current_node->item;
    }
    else
    {
        pList->status = LIST_OOB_START;
        pList->current_node = NULL;
        return NULL;
    }
}

// Returns a pointer to the current item in pList.
void *List_curr(List *pList)
{
    assert(pList != NULL);
    if(pList -> current_node == NULL){
        return NULL;
    }
    return pList->current_node->item;
}

// Adds the new item to pList directly after the current item, and makes item the current item.
// If the current pointer is before the start of the pList, the item is added at the start. If
// the current pointer is beyond the end of the pList, the item is added at the end.
// Returns 0 on success, -1 on failure.
int List_insert_after(List *pList, void *pItem)
{
    assert(pList != NULL);
    if (node_stack_top == NULL)
    {
        return LIST_FAIL;
    }
    // initializing the front of the stack
    Node* temp = node_stack_top;
    node_stack_top->item = pItem;
    pop_node();

    // if the current stutus is before the start
    if (pList->status == LIST_OOB_START)
    {
        if(pList -> node_num == 0){ // if the list is empty
            pList -> head = temp;
            pList -> tail = temp;
            pList -> current_node = temp;
            temp -> next = NULL;
            temp -> pre = NULL;
            
        }else{ // if not empty
            pList -> head -> pre = temp;
            temp -> next = pList -> head;
            pList -> head = temp;
            pList -> current_node = temp;
        }
        pList -> node_num++;
        pList -> status = LIST_OOB_INSIDE;
        return LIST_SUCCESS;
    }
    // if the current_item is pointing at after at the end
    if (pList->status == LIST_OOB_END)
    {
        if(pList -> node_num == 0){ // if the list is empty
            pList -> head = temp;
            pList -> tail = temp;
            pList -> current_node = temp;
            temp -> next = NULL;
            temp -> pre = NULL;
        }else{ // if not empty
            pList -> tail -> next = temp;
            temp -> pre = pList -> tail;
            pList -> current_node = temp;
            pList -> tail = temp;
        }
        pList -> node_num++;
        pList -> status = LIST_OOB_INSIDE;
        return LIST_SUCCESS;
    }
    else
    { // after the current item
        // if there is no node after the current node
        if (pList->current_node->next == NULL)
        {
            temp -> pre = pList -> current_node;
            pList -> current_node -> next = temp;
            pList -> tail = temp;
            pList -> current_node = temp;
        }
        else
        { // if there is a node after the current node
            temp -> pre = pList -> current_node;
            temp -> next = pList -> current_node -> next; 
            pList -> current_node -> next = temp;
            temp -> next -> pre = temp;
            pList -> current_node = temp;
        }
        pList -> node_num++;
        return LIST_SUCCESS;
    }
}

// Adds item to pList directly before the current item, and makes the new item the current one.
// If the current pointer is before the start of the pList, the item is added at the start.
// If the current pointer is beyond the end of the pList, the item is added at the end.
// Returns 0 on success, -1 on failure.
int List_insert_before(List *pList, void *pItem)
{

    // if there are no more avaliable nodes
    if (node_stack_top == NULL)
    {
        return LIST_FAIL;
    }

    Node* temp = node_stack_top;
    node_stack_top->item = pItem;
    pop_node();

    // if the current_item is pointing at before the start
    if (pList->status == LIST_OOB_START)
    {
        if(pList -> node_num == 0){ // if the list is empty
            pList -> head = temp;
            pList -> tail = temp;
            pList -> current_node = temp;
            temp -> next = NULL;
            temp -> pre = NULL;
        }else{ // if not empty
            pList -> head -> pre = temp;
            temp -> next = pList -> head;
            pList -> head = temp;
            pList -> current_node = temp;
        }
        pList -> node_num++;
        pList -> status = LIST_OOB_INSIDE;
        return LIST_SUCCESS;
    }
    // if the current_item is pointing at after at the end
    if (pList->status == LIST_OOB_END)
    {
        if(pList -> node_num == 0){ // if the list is empty
            pList -> head = temp;
            pList -> tail = temp;
            pList -> current_node = temp;
            temp -> next = NULL;
            temp -> pre = NULL;
        }else{ // if not empty
            pList -> tail -> next = temp;
            temp -> pre = pList -> tail;
            pList -> current_node = temp;
            pList -> tail = temp;
        }
        pList -> node_num++;
        pList -> status = LIST_OOB_INSIDE;
        return LIST_SUCCESS;
    }
    else
    { // before the current item
        // if there is no node before the current node
        if (pList->current_node->pre == NULL)
        {
            temp -> next = pList -> head;
            pList -> head -> pre = temp;
            pList -> head = temp;
            pList -> current_node = pList -> head;
   
        }
        else // if there is a node before the current node
        {
            temp -> pre = pList -> current_node -> pre;
            temp -> next = pList -> current_node;
            pList -> current_node -> pre = temp;
            temp -> pre -> next = temp;
            pList -> current_node = temp;
        }
        pList -> node_num++;
        return LIST_SUCCESS;
    }
}

// Adds item to the end of pList, and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int List_append(List *pList, void *pItem)
{
    assert(pList != NULL);
    if (node_stack_top == NULL)
    {
        return LIST_FAIL;
    }

    node_stack_top -> item = pItem;
    pList -> status = LIST_OOB_INSIDE;
    // two cases: first item in the list or anything else
    if (pList->node_num == 0)
    {
        pList->head = node_stack_top;
        pop_node();
        pList->tail = pList->head;
        pList->current_node = pList->tail;
        pList->node_num++;
        return LIST_SUCCESS;
    }
    else
    { // if it is not the first item in the list
        pList->tail->next = node_stack_top;
        pop_node();
        pList->tail->next->pre = pList->tail;
        pList->tail = pList->tail->next;
        pList->current_node = pList->tail;
        pList->node_num++;
        return LIST_SUCCESS;
    }
    
}

// Adds item to the front of pList, and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int List_prepend(List *pList, void *pItem)
{
    assert(pList != NULL);
    if (node_stack_top == NULL)
    {
        return LIST_FAIL;
    }
    Node *temp = node_stack_top;
    pop_node();
    pList->node_num++;
    temp->item = pItem;
    pList->current_node = temp;

    // two case: list is empty or list is not empty
    if (pList->head == NULL)
    { // if pList is empty
        pList->head = temp;
        pList->tail = temp;
        pList->status = LIST_OOB_START;
        return LIST_SUCCESS;
    }
    else
    { // if pList is not empty
        pList->head->pre = temp;
        pList->head->pre->next = pList->head;
        pList->head = temp;
        pList->current_node = temp;
        pList->status = LIST_OOB_INSIDE;
        return LIST_SUCCESS;
    }
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void *List_remove(List *pList)
{
    assert(pList != NULL);
    if (pList->status == LIST_OOB_START || pList->status == LIST_OOB_END || pList -> current_node == NULL)
    {
        return NULL;
    }

    // if there is nothing in the list, then it must be the case that the current_node is pointing at NULL

    Node* temp = pList -> current_node;

    if(temp == NULL){
        return NULL;
    }
    
    // 3 case: 1 node, 2 nodes, and more then 3 nodes
    // if the list only have one item
    if (pList->node_num == 1)
    {
        // re-init the list
        pList->head = NULL;
        pList->tail = NULL;
        pList->current_node = NULL;
        pList->status = LIST_OOB_END;
        pList->node_num--;
        push_node(temp);
        return NULL;
    }

    // if there is a next node and the current node is the head
    if (temp -> next != NULL && temp == pList -> head){
        // if it is not
        pList->head = temp->next;
        pList->current_node = pList->head;
        pList->current_node->pre = NULL;
    }
    
    // if there is a pre node and the current node is the tail
    // special special case
    if (temp -> pre != NULL && temp == pList -> tail){
        pList -> tail = temp -> pre; // set tail to tail -> pre
        pList -> current_node = NULL;
        pList -> tail -> next = NULL;
        pList -> status = LIST_OOB_END;
        pList -> node_num--;
        push_node(temp);
        return NULL;
    }

    // if (node_num >= 3)
    // if the current_node is inbetween of two other nodes
    if (pList->current_node->pre != NULL && pList->current_node->next != NULL){
        temp -> pre -> next = temp -> next;
        temp -> next -> pre = temp -> pre;
        pList -> current_node = temp -> next;
    }

    // Set status
    pList->status = LIST_OOB_INSIDE;

    pList->node_num--;
    // putting the node back into the stack
    push_node(temp);

    return temp -> item;
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList){
    assert(pList != NULL);
    if(pList -> node_num == 0){
        return NULL;
    }
    Node* temp = pList -> tail;
    // 2 case: Only one node in the list or more than one
    // only one node
    if(pList -> node_num == 1){
        pList -> head = NULL;
        pList -> tail = NULL;
        pList -> current_node = NULL;
        pList -> status = LIST_OOB_START;
    } else { // more than one node
        pList -> tail = temp -> pre; // set tail to tail -> pre
        pList -> current_node = pList -> tail;
        pList -> tail -> next = NULL;
        pList->status = LIST_OOB_INSIDE;
    }

    pList -> node_num--;
    push_node(temp);

    return temp -> item;
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2){
    // 1-2-3 + 1-2-3 == 1-2-3-1-2-3
    // make sure pList1 and pList2 isnt emtpy
    assert(pList2 != NULL);
    assert(pList1 != NULL);
    if(pList1 -> node_num == 0){
        pList1 -> head = pList2 -> head;
        pList1 -> tail = pList2 -> tail;
        pList1 -> node_num += pList2 -> node_num;
        push_list(pList2);
        return;
    }
    if(pList2 -> node_num == 0){
        push_list(pList2);
        return;
    }
    // save a copy of pList2
    List* temp = pList2;
    pList1 -> tail -> next = pList2 -> head;
    pList2 -> head -> pre = pList1 -> tail;
    pList1 -> node_num += pList2 -> node_num;
    pList1 -> tail = pList2 -> tail;

    // discarding pList2 head
    // free_list
    push_list(temp);
    // removing nodes in list?
}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
typedef void (*FREE_FN)(void* pItem);
void List_free(List* pList, FREE_FN pItemFreeFn){
    // remove all nodes
    assert(pList != NULL);
    while(pList -> tail != NULL){
        // Invoke
        (*pItemFreeFn)(pList -> tail -> item);
        // remove every last item
        List_trim(pList);
    }

    // re-init list and push it back to the stack
    pList -> head = NULL;
    pList -> tail = NULL;
    pList -> current_node = NULL;
    pList -> status = LIST_OOB_START;
    pList -> node_num = 0;
    push_list(pList);
}

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    assert(pList != NULL);
    // base case
    if(pList -> status == LIST_OOB_END){
        return NULL;
    }
    if(pList -> status == LIST_OOB_START){
        if(pList -> head == NULL)
            return NULL;
        pList -> current_node = pList -> head;
    }

    // start from the current_node;
    Node* temp = pList -> current_node;
    while(temp != NULL){
        if((*pComparator)(temp->item, &pComparisonArg)) {
            // if found
            pList -> current_node = temp;
            return pList -> current_node -> item;
        }
    
        temp = temp -> next;
    }
    // if not
    // left the pointer as beyond the end of the list
    pList -> current_node = NULL;
    pList -> status = LIST_OOB_END;
    return NULL;
}
