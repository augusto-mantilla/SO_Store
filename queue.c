#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue* create_queue(int max_elements)
{
  Queue *Q = (Queue*)malloc(sizeof(Queue));
  Q->elements = (int*)malloc(sizeof(int)*max_elements);
  Q->size = 0;
  Q->capacity = max_elements;
  Q->front = 0;
  Q->rear = -1;
  return Q;
}

void dequeue(Queue *Q)
{
  if(Q->size == 0)
    {
    printf("A lista está vazia\n");
    return;
    }
  else
    {
      Q->size--;
      Q->front = (Q->front + 1)% Q->capacity ;
    }
  return;
}

void enqueue(Queue *Q, int element)
{
  if(Q->size == Q->capacity)
    {
      printf("A fila está cheia\n");
    }
  else
    {
      Q->size++;
      Q->rear = (Q->rear + 1)% Q->capacity;
      Q->elements[Q->rear] = element;
    }
  return;
}

int isin(Queue *Q, int element)
{
  int i = Q->front;
  do
    {
      if(Q->elements[i] == element)
	{
	  return 1;
	}
      i = (i + 1) % Q->capacity;
    }
  while(i != (Q->rear + 1)%Q->capacity);
  return 0;
}

void print_queue(Queue *Q)
{
  int i = Q->front;
  int j = 0;
  do
    {
      if(Q->size != 0)
	{
	  printf("Elemento %d:%d\n", j, Q->elements[i]);
	  if(i < Q->capacity - 1)
	    i++;
	  else
	    i = 0;
	  j++;
	}
      else
	{
	  break;
	}
    }
  while(i != (Q->rear + 1) % Q->capacity);
}

int queue_length(Queue *Q)
{
  return Q->size;
}
