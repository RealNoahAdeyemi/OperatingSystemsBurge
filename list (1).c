// list/list.c
// Generated using Chat GPT
// Implementation for linked list.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "list.h"

list_t *list_alloc() { 
  list_t* list = (list_t*)malloc(sizeof(list_t));
  list->head = NULL;
  return list; 
}

node_t *node_alloc(block_t *blk) {   
  node_t* node = malloc(sizeof(node_t));
  node->next = NULL;
  node->blk = blk;
  return node; 
}

void list_free(list_t *l){
  if (!l) return;
  node_t *cur = l->head;
  while (cur) {
    node_t *next = cur->next;
    if (cur->blk) free(cur->blk);
    free(cur);
    cur = next;
  }
  free(l);
  return;
}

void node_free(node_t *node){
  if (!node) return;
  free(node);
  return;
}

void list_print(list_t *l) {
  node_t *current = l->head;
  block_t *b;
  
  if (current == NULL){
    printf("list is empty\n");
    return;
  }
  while (current != NULL){
    b = current->blk;
    printf("PID=%d START:%d END:%d\n", b->pid, b->start, b->end);
    current = current->next;
  }
}

int list_length(list_t *l) { 
  node_t *current = l->head;
  int i = 0;
  while (current != NULL){
    i++;
    current = current->next;
  }
  
  return i; 
}

void list_add_to_back(list_t *l, block_t *blk){  
  node_t* newNode = node_alloc(blk);
  newNode->next = NULL;
  if(l->head == NULL){
    l->head = newNode;
  }
  else{
    node_t *current = l->head;
    while(current->next != NULL){
      current = current->next;
    }
    current->next = newNode;
  }
}

void list_add_to_front(list_t *l, block_t *blk){  
  node_t* newNode = node_alloc(blk);
  newNode->next = l->head;
  l->head = newNode;
}

void list_add_at_index(list_t *l, block_t *blk, int index){
  if (index <= 0 || l->head == NULL) {
    list_add_to_front(l, blk);
    return;
  }
  
  int i = 0;
  node_t *newNode = node_alloc(blk);
  node_t *current = l->head;

  while(i < index-1 && current->next != NULL){
    current = current->next;
    i++;
  }
  newNode->next = current->next;
  current->next = newNode;
}

void list_add_ascending_by_address(list_t *l, block_t *newblk){
    node_t *newNode = node_alloc(newblk);
    newNode->next = NULL;

    if (l->head == NULL) {
        l->head = newNode;
        return;
    }

    // Insert at head if start is smallest
    if (newblk->start < l->head->blk->start) {
        newNode->next = l->head;
        l->head = newNode;
        return;
    }

    node_t *cur = l->head;
    while (cur->next != NULL && cur->next->blk->start < newblk->start) {
        cur = cur->next;
    }
    newNode->next = cur->next;
    cur->next = newNode;
}

void list_add_ascending_by_blocksize(list_t *l, block_t *newblk){
   node_t *newNode = node_alloc(newblk);
   newNode->next = NULL;
   int new_size = (newblk->end - newblk->start) + 1;

   if (l->head == NULL) {
       l->head = newNode;
       return;
   }

   // Insert at head if smaller than head
   int head_size = (l->head->blk->end - l->head->blk->start) + 1;
   if (new_size < head_size || (new_size == head_size && newblk->start < l->head->blk->start)) {
       newNode->next = l->head;
       l->head = newNode;
       return;
   }

   node_t *cur = l->head;
   while (cur->next != NULL) {
       int next_size = (cur->next->blk->end - cur->next->blk->start) + 1;
       if (new_size < next_size || (new_size == next_size && newblk->start < cur->next->blk->start)) {
           break;
       }
       cur = cur->next;
   }
   newNode->next = cur->next;
   cur->next = newNode;
}

void list_add_descending_by_blocksize(list_t *l, block_t *blk){
  node_t *current;
  node_t *prev;
  node_t *newNode = node_alloc(blk);
  int newblk_size = (blk->end - blk->start) + 1;
  int curblk_size;
  
  if(l->head == NULL){
    l->head = newNode;
    return;
  }
  else{
    prev = current = l->head;
    curblk_size = (current->blk->end - current->blk->start) + 1;
    
    if(current->next == NULL) {  //only one node in list
       if(newblk_size >= curblk_size) {  // place in front of current node
          newNode->next = l->head;
          l->head = newNode;
       }
       else {   // place behind current node
          current->next = newNode;
          newNode->next = NULL;
       }
    }
    else {  // two or more nodes in list
      
       if(newblk_size >= curblk_size) {  // place in front of current node
          newNode->next = l->head;
          l->head = newNode;
       }
       else {
          while(current != NULL) {
               curblk_size = (current->blk->end - current->blk->start) + 1;
               if (newblk_size >= curblk_size) break;
               prev = current;
               current = current->next;
          }
          prev->next = newNode;
          newNode->next = current;
       }
    }
  }
}

void list_coalese_nodes(list_t *l){ 
  if (!l || !l->head) return;

  node_t *prev = l->head;
  node_t *cur = prev->next;

  while (cur != NULL) {
      // If prev and cur are adjacent: prev.end + 1 == cur.start
      if (prev->blk->end + 1 == cur->blk->start) {
          // Merge cur into prev: extend prev->blk->end
          prev->blk->end = cur->blk->end;

          // Remove cur node and free its blk
          node_t *tmp = cur;
          cur = cur->next;
          prev->next = cur;

          if (tmp->blk) free(tmp->blk);
          node_free(tmp);
      } else {
          prev = cur;
          cur = cur->next;
      }
  }
}

block_t* list_remove_from_back(list_t *l){
  block_t *value = NULL;
  if (l == NULL || l->head == NULL) return NULL;

  node_t *current = l->head;
  node_t *prev = NULL;

  // single node
  if (current->next == NULL) {
    value = current->blk;
    l->head = NULL;
    node_free(current);
    return value;
  }

  // multiple nodes: find last
  while (current->next != NULL) {
    prev = current;
    current = current->next;
  }
  // current is last, prev is second-last
  value = current->blk;
  prev->next = NULL;
  node_free(current);
  return value;
}

block_t* list_get_from_front(list_t *l) {
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *current = l->head;
    value = current->blk;
  }
  return value; 
}


block_t* list_remove_from_front(list_t *l) { 
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *current = l->head;
    value = current->blk;
    l->head = l->head->next;
    node_free(current);
  }
  return value; 
}

block_t* list_remove_at_index(list_t *l, int index) { 
  int i;
  block_t* value = NULL;
  
  bool found = false;
  
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_remove_from_front(l);
  }
  else if (index > 0){
    node_t *current = l->head;
    node_t *prev = current;
    
    i = 0;
    while(current != NULL && !found){
      if(i == index) found = true;
      else {
         prev = current;
         current = current->next;
         i++;
      }
    }
    
    if(found && current != NULL) {
      value = current->blk; 
      prev->next = current->next;
      node_free(current);
    }
  }
  return value; 
}

bool compareBlks(block_t* a, block_t *b) {
  if(a->pid == b->pid && a->start == b->start && a->end == b->end)
     return true;
  return false;
}

bool compareSize(int a, block_t *b) {  // greater or equal
  if(a <= (b->end - b->start) + 1)
     return true;
  return false;
}

bool comparePid(int a, block_t *b) {
  if(a == b->pid)
     return true;
  return false;
}


bool list_is_in(list_t *l, block_t* value) { 
  node_t *current = l->head;
  while(current != NULL){
    if(compareBlks(value, current->blk)){
      return true;
    }
    current = current->next;
  }
return false; 
}

block_t* list_get_elem_at(list_t *l, int index) { 
 int i;
  block_t* value = NULL;
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_get_from_front(l);
  }
  else if (index > 0){
    node_t *current = l->head;
    
    i = 0;
    while(current != NULL){
      if(i == index)
          return(current->blk);
      else {
         current = current->next;
         i++;
      }
    }
  }
  return value; 
}

int list_get_index_of(list_t *l, block_t* value){
 int i = 0;
 node_t *current = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (current != NULL){
   if (compareBlks(value,current->blk)){
     return i;
    }
    current = current->next;
    i++;
  }
  return -1; 
}

/* Checks to see if block of Size or greater exists in the list. */
bool list_is_in_by_size(list_t *l, int Size){ 
  node_t *current = l->head;
  while(current != NULL){
    if(compareSize(Size, current->blk)){
      return true;
    }
    current = current->next;
  }
return false; 
}

/* Checks to see if pid of block exists in the list. */
bool list_is_in_by_pid(list_t *l, int pid){ 
  node_t *current = l->head;
  while(current != NULL){
    if(comparePid(pid, current->blk)) {
      return true;
    }
    current = current->next;
  }
  return false;
}

/* Returns the index at which the given block of Size or greater appears. */
int list_get_index_of_by_Size(list_t *l, int Size){
 int i = 0;
 node_t *current = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (current != NULL){
   if (compareSize(Size,current->blk)){
     return i;
    }
    current = current->next;
    i++;
  }

  return -1; 
}
                   
/* Returns the index at which the pid appears. */
int list_get_index_of_by_Pid(list_t *l, int pid){
 int i = 0;
 node_t *current = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (current != NULL){
   if (comparePid(pid,current->blk)){
     return i;
    }
    current = current->next;
    i++;
  }

  return -1; 
}
