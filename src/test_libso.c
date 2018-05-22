#include <semaphore.h>

#include "libso.h"

int main() 
{
	void* tptr = memory_create("test_libso_memory", 100);
	memory_destroy("test_libso_memory", tptr, 100);
	
	sem_t* tsemtptr = semaphore_create("test_libso_semaphore", 0);
	semaphore_destroy("test_libso_semaphore", tsemtptr);
}
