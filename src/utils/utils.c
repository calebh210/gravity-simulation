#include <stdlib.h>
#include <stdio.h>
#include "utils/utils.h"

points_list* init_list(){

    points_list *pL = (points_list *)malloc(sizeof(points_list));
    if(pL == NULL){
        printf("Failed to allocate memory for Points List \n");
        exit(1);
    }
    pL->head = NULL;
    pL->count = 0;
    return pL;
}

//add point p to the tail of the list
void add_to_list(points_list *pL, point *p){
    //check if the data is similar. if it's within a certain range, don't add it.

    if(pL->head == NULL){
        pL->head = p;
        p->next = NULL;
        pL->count++;
        return;
    }

    point *iter = pL->head;

    while(iter->next != NULL){
        iter = iter->next;
    }

    iter->next = p;
    p->next = NULL;
    pL->count++;

                
}

// add new point P to head of list
void add_to_list_head(points_list *pL, point *p){
    //check if the data is similar. if it's within a certain range, don't add it.
    p->next = pL->head;
    if(pL->head == NULL){
        pL->count = 1;
    } else {
        pL->count++;
    }
    pL->head = p;    
}

void free_list(points_list *pL){
    point *iterator;   
    while(pL->head != NULL){
        iterator = pL->head;
        pL->head = pL->head->next;
        free(iterator);
    }
}