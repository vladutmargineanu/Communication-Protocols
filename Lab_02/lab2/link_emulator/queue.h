#ifndef QUEUE
#define QUEUE
#include "lib.h"

// Crearea unui nod (lista simplu inlantuita)
struct q {
  void* crt;  // cheia
  struct q* prev;
};

typedef struct q queue_entry;

typedef struct {
  int size;
  queue_entry* first;
  queue_entry* last;
} queue;

void enqueue(queue* q, void* m);
void* dequeue(queue* q);

queue* create_queue();
void destroy_queue(queue* q);

#endif
