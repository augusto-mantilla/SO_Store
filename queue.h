#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
  int capacity;
  int size;
  int front;
  int rear;
  int *elements;
}Queue;

//cria uma fila implementada com arrays (recebe o número máximo de elementos)
Queue* create_queue(int max_elements);

//retira o primeiro elemento da fila
void dequeue(Queue *Q);

//introduz um elemento na fila
void enqueue(Queue *Q, int element);

//retorna 0 se 'element' ocorre na fila 'Q'
int isin(Queue *Q, int element);

//escreve no terminal todos os elementos da fila 'Q'
void print_queue(Queue *Q);

#endif
