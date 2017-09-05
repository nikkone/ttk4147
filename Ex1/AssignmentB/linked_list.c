#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

struct node* list_get_node(list_t list, int index);

list_t list_create()
{
	list_t new_list = (list_t) malloc(sizeof(struct list));
	new_list->head = NULL;
	new_list->tail = NULL;
	new_list->length = 0;
	return new_list;
}

void list_delete(list_t list)
{
	struct node *tbd = list->tail;
	struct node *temp;
	while(tbd != NULL){
		temp = tbd;
		tbd = temp->prev;
		free(temp);
	}
	free(list);
}

void list_insert(list_t list, int index, int data)
{
	struct node *temp = list_get_node(list,index);
	struct node *tba = (struct node*) malloc(sizeof(struct node));
	tba->data = data;
	tba->next = temp;
	tba->prev = temp->prev;
	(temp->prev)->next = tba;
	temp->prev = tba;
	(list->length)++;
}

void list_append(list_t list, int data)
{
	struct node* tba = (struct node*) malloc(sizeof(struct node));
	tba->data = data;
	tba->next = NULL;
	tba->prev = list->tail;
	if (list->head == NULL){
		list->head = tba;
	}else{
		(list->tail)->next = tba;
	}
	list->tail = tba;
	(list->length)++;
}

void list_print(list_t list)
{
	int i;
	struct node *tbp = list->head;
	if(tbp != NULL){
		for(i = 0; i < list->length; i++){
			printf("%i ", list_get(list,i));
			tbp = tbp->next;
		}
	}
	printf("\n");
}

long list_sum(list_t list)
{
	long sum = 0;
	struct node *temp = list->head;
	while(temp != NULL){
		sum += temp->data;
		temp = temp->next;
	}
	return sum;
}

struct node* list_get_node(list_t list, int index)
{
	int i;
	struct node *temp = list->head; 
	for(i = 1; i <= index; i++){
		temp = temp->next;
	}
	return temp;
}

int list_get(list_t list, int index)
{
	return list_get_node(list,index)->data;
}

int list_extract(list_t list, int index)
{
	int data;
	struct node *temp = list_get_node(list, index);
	data = temp->data;
	if (index != 0){
		(temp->prev)->next = temp->next;
		(temp->next)->prev = temp->prev;
	} else {
		list->head = temp->next;
		if (temp->next != NULL)
			(temp->next)->prev = NULL;
		else
			list->tail = NULL;
	}
	free(temp);
	(list->length)--;
	return data;
}
