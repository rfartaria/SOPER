#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/mman.h> //mmap
#include <sys/stat.h>
#include "libso.h"

void * memory_create(char * name, int size) {
    //==============================================
    // FUNÇÃO GENÉRICA DE CRIAÇÃO DE MEMÓRIA PARTILHADA
    //
    // usar getuid() para gerar nome unico na forma:
    // sprintf(name_uid,"/%s_%d", name, getuid())
    // usar name_uid em shm_open
    // usar tambem: ftruncate e mmap
    //return so_memory_create(name, size);
    //==============================================
    int fd;
    char* name_uid;
    char* error_message;
    void* ptr;
    #define LSTRSIZE 200

    name_uid = (char*)calloc(LSTRSIZE,1);
    error_message = (char*)calloc(LSTRSIZE,1);
    
    sprintf(name_uid,"/%s_%d", name, getuid());
    fd = shm_open(name_uid, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {

        memset(error_message, 0, LSTRSIZE);
        strcpy(error_message, "shm_open: ");
        strcat(error_message, name_uid);
        perror(error_message);
        exit(10);
    }
    
    if (ftruncate(fd, size) == -1) 
    {
        shm_unlink(name_uid); // se houve erro, elimina ficheiro
        memset(error_message, 0, LSTRSIZE);
        strcpy(error_message, "ftruncate : ");
        strcat(error_message, name_uid);
        perror(error_message);
        exit(11);
    }
    
    ptr = mmap(0, 1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        shm_unlink(name_uid); // se houve erro, elimina ficheiro
        memset(error_message, 0, LSTRSIZE);
        strcpy(error_message, "mmap : ");
        strcat(error_message, name_uid);
        perror(error_message);
        exit(12);
    }
    
    free(name_uid);
    free(error_message);
    
    return ptr;
}


void memory_destroy(char * name, void * ptr, int size) {
    //==============================================
    // FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE MEMÓRIA PARTILHADA
    //
    // usar getuid() para gerar nome unico na forma:
    // sprintf(name_uid,"/%s_%d", name, getuid())
    // usar name_uid em shm_unlink
    // usar tambem: munmap
    // so_memory_destroy(name, ptr, size);
    //==============================================
    char* name_uid;
    char* error_message;
    #define LSTRSIZE 200

    name_uid = (char*)calloc(LSTRSIZE,1);
    error_message = (char*)calloc(LSTRSIZE,1);
    
    sprintf(name_uid,"/%s_%d", name, getuid());
    
    if (munmap(ptr, size) == -1)
    {
        memset(error_message, 0, LSTRSIZE);
        strcpy(error_message, "munmap : ");
        strcat(error_message, name_uid);
        perror(error_message);
        exit(13);
    }

    if (shm_unlink(name_uid) == -1) 
    {
        memset(error_message, 0, LSTRSIZE);
        strcpy(error_message, "shm_unlink : ");
        strcat(error_message, name_uid);
        perror(error_message);
        exit(14);
    }
    
    free(name_uid);
    free(error_message);
}


sem_t * semaphore_create(char * name, int value) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE UM SEMÁFORO
	//
	// return so_semaphore_create(name, value);
    char name_uid[100] = {0};
    char error_msg[1000] = {0};
    sprintf(name_uid,"/%s_%d", name, getuid());
    
    sem_t* semaphore = sem_open(name_uid, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, value);
	if (semaphore == SEM_FAILED)
	{
            sprintf(error_msg, "semaphore_create : %s", name_uid);
			perror(error_msg);
			exit(16);
	}
	return semaphore;
	//==============================================
}


void semaphore_destroy(char * name, void * ptr) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE UM SEMÁFORO
	//
	//so_semaphore_destroy(name, ptr);
    char name_uid[100] = {0};
    sprintf(name_uid,"/%s_%d", name, getuid());
	
	char error_msg[100] = {0};
	
	if (sem_close(ptr) == -1)
	{
		sprintf(error_msg, "semaphore_destroy sem_close: %s", name_uid);
		perror(error_msg);
		exit(17);
	}
	
	if (sem_unlink(name_uid) == -1) 
	{
		sprintf(error_msg, "semaphore_destroy sem_unlink: %s", name_uid);
		perror(error_msg);
		exit(18);
	}
	//==============================================
}
