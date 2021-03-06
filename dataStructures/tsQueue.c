#include "tsQueue.h"
#include <stdlib.h>


TSQueue *tsq_create() {
  TSQueue *tsq = (TSQueue *) malloc(sizeof(TSQueue));
  if (tsq == NULL) return NULL;
  pthread_mutex_init(&(tsq->lock), NULL);
  pthread_cond_init(&(tsq->cond), NULL);
  tsq->elem = NULL;
  return tsq;
}

void tsq_destroy(TSQueue *tsq, void (*payloadFree)(void *payload)) {
  Elem *e, *f;
  pthread_mutex_lock(&(tsq->lock));
  e = tsq->elem;
  while (e != NULL) {
    f = e;
    e = f->next;
    if (payloadFree) payloadFree(f->payload);
    free(f);
  }
  pthread_mutex_unlock(&(tsq->lock));
  free(tsq);
}

void *tsq_get(TSQueue *tsq) {
  void *payload = NULL;
  Elem *e = NULL;
  pthread_mutex_lock(&(tsq->lock));
  while (tsq->elem == NULL) pthread_cond_wait(&(tsq->cond), &(tsq->lock));
  e = tsq->elem;
  payload = e->payload;
  tsq->elem = e->next;
  pthread_mutex_unlock(&(tsq->lock));
  free(e);
  return payload;
}

int tsq_put(TSQueue *tsq, void *payload) {
  Elem *e;
  Elem *elem = (Elem *) malloc(sizeof(Elem));
  if (elem == NULL) return 0;
  elem->payload = payload;
  elem->next = NULL;

  pthread_mutex_lock(&(tsq->lock));
  if (tsq->elem == NULL) tsq->elem = elem;
  else { // add to the end of the queue
    for(e = tsq->elem; e->next != NULL; e = e->next);
    e->next = elem;
  }
  pthread_cond_signal(&(tsq->cond));
  pthread_mutex_unlock(&(tsq->lock));
  return 1;
}

/**
 * Find the last element matching the key using the specified equal function.
 */
void *tsq_find(TSQueue *tsq, int (*equal_fn)(void *a, void *b), void *key) {
  void *value = NULL;
  pthread_mutex_lock(&(tsq->lock));
  for(Elem *e = tsq->elem; e != NULL; e = e->next) {
    if (equal_fn(key, e->payload)) value = e->payload;
  }
  pthread_mutex_unlock(&(tsq->lock));
  return value;
}
