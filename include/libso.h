#ifndef LIBSO_H_GUARD
#define LIBSO_H_GUARD

void * memory_create(char*, int);
void memory_destroy(char *, void *, int);
sem_t * semaphore_create(char*, int);
void semaphore_destroy(char *, void *);

#endif
