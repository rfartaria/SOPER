#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h> //mmap
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "prodcons.h"
#include "control.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct prodcons ProdCons;

//******************************************
// SEMAFORO_CRIAR
//
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

void prodcons_create_stock() {
	//==============================================
	// CRIAR MUTEX PARA CONTROLAR O ACESSO AO STOCK
	//
	// utilizar a função genérica semaphore_create
	// so_prodcons_create_stock();
    ProdCons.stock_mutex = semaphore_create(STR_SEM_STOCK_MUTEX, 1);
	//==============================================
}

void prodcons_create_buffers() {
	//==============================================
	// CRIAR SEMAFOROS PARA CONTROLAR O ACESSO AOS 3 BUFFERS
	//
	// utilizar a função genérica semaphore_create
	// so_prodcons_create_buffers();
    ProdCons.request_p_full = semaphore_create(STR_SEM_REQUEST_FULL, 0);
	ProdCons.request_p_empty = semaphore_create(STR_SEM_REQUEST_EMPTY, Config.BUFFER_REQUEST);
	ProdCons.request_p_mutex = semaphore_create(STR_SEM_REQUEST_MUTEX, 1);
	
	ProdCons.request_e_full = semaphore_create(STR_SEM_ORDER_FULL, 0);
	ProdCons.request_e_empty = semaphore_create(STR_SEM_ORDER_EMPTY, Config.BUFFER_ORDER);
	ProdCons.request_e_mutex = semaphore_create(STR_SEM_ORDER_MUTEX, 1);
	
	ProdCons.receipts_r_full = semaphore_create(STR_SEM_RECEIPT_FULL, 0);
	ProdCons.receipts_r_empty = semaphore_create(STR_SEM_RECEIPT_EMPTY, Config.BUFFER_RECEIPT);
	ProdCons.receipts_r_mutex = semaphore_create(STR_SEM_RECEIPT_MUTEX, 1);
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

void prodcons_destroy() {
	//==============================================
	// DESTRUIR SEMÁFORO E RESPETIVO NOME
	//
	// utilizar a função genérica semaphore_destroy
	//so_prodcons_destroy();
    semaphore_destroy(STR_SEM_STOCK_MUTEX, ProdCons.stock_mutex);
	
	semaphore_destroy(STR_SEM_REQUEST_FULL, ProdCons.request_p_full);
	semaphore_destroy(STR_SEM_REQUEST_EMPTY, ProdCons.request_p_empty);
	semaphore_destroy(STR_SEM_REQUEST_MUTEX, ProdCons.request_p_mutex);
	
	semaphore_destroy(STR_SEM_ORDER_FULL, ProdCons.request_e_full);
	semaphore_destroy(STR_SEM_ORDER_EMPTY, ProdCons.request_e_empty);
	semaphore_destroy(STR_SEM_ORDER_MUTEX, ProdCons.request_e_mutex);
	
	semaphore_destroy(STR_SEM_RECEIPT_FULL, ProdCons.receipts_r_full);
	semaphore_destroy(STR_SEM_RECEIPT_EMPTY, ProdCons.receipts_r_empty);
	semaphore_destroy(STR_SEM_RECEIPT_MUTEX, ProdCons.receipts_r_mutex);
	//==============================================
}

//******************************************
void prodcons_request_p_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER PEDIDO DE CURRENCY
	//
	//so_prodcons_request_p_produce_begin();
    if (sem_wait(ProdCons.request_p_empty) == -1)
	{
		perror("sem_wait : ProdCons.request_p_empty");
		exit(20);
	}
	
	if (sem_wait(ProdCons.request_p_mutex) == -1)
	{
		perror("sem_wait : ProdCons.request_p_mutex");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_request_p_produce_end() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER PEDIDO DE CURRENCY
	//
	//so_prodcons_request_p_produce_end();
    if (sem_post(ProdCons.request_p_mutex) == -1)
	{
		perror("sem_post : ProdCons.request_p_mutex");
		exit(20);
	}
	
	if (sem_post(ProdCons.request_p_full) == -1)
	{
		perror("sem_post : ProdCons.request_p_full");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_request_p_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE CURRENCY
	//
	//so_prodcons_request_p_consume_begin();
    if (sem_wait(ProdCons.request_p_full) == -1)
	{
		perror("sem_wait : ProdCons.request_p_full");
		exit(20);
	}
	
	if (sem_wait(ProdCons.request_p_mutex) == -1)
	{
		perror("sem_wait : ProdCons.request_p_mutex");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_request_p_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE CURRENCY
	//
	//so_prodcons_request_p_consume_end();
    if (sem_post(ProdCons.request_p_mutex) == -1)
	{
		perror("sem_post : ProdCons.request_p_mutex");
		exit(20);
	}
	
	if (sem_post(ProdCons.request_p_empty) == -1)
	{
		perror("sem_post : ProdCons.request_p_empty");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_request_e_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER PEDIDO DE ENCOMENDA
	//
	//so_prodcons_request_e_produce_begin();
    if (sem_wait(ProdCons.request_e_empty) == -1)
	{
		perror("sem_wait : ProdCons.request_e_empty");
		exit(20);
	}
	
	if (sem_wait(ProdCons.request_e_mutex) == -1)
	{
		perror("sem_wait : ProdCons.request_e_mutex");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_request_e_produce_end() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER PEDIDO DE ENCOMENDA
	//
	//so_prodcons_request_e_produce_end();
    if (sem_post(ProdCons.request_e_mutex) == -1)
	{
		perror("sem_post : ProdCons.request_e_mutex");
		exit(20);
	}
	
	if (sem_post(ProdCons.request_e_full) == -1)
	{
		perror("sem_post : ProdCons.request_e_full");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_request_e_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE ENCOMENDA
	//
	//so_prodcons_request_e_consume_begin();
    if (sem_wait(ProdCons.request_e_full) == -1)
	{
		perror("sem_wait : ProdCons.request_e_full");
		exit(20);
	}
	
	if (sem_wait(ProdCons.request_e_mutex) == -1)
	{
		perror("sem_wait : ProdCons.request_e_mutex");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_request_e_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE ENCOMENDA
	//
	// so_prodcons_request_e_consume_end();
    if (sem_post(ProdCons.request_e_mutex) == -1)
	{
		perror("sem_post : ProdCons.request_e_mutex");
		exit(20);
	}
	
	if (sem_post(ProdCons.request_e_empty) == -1)
	{
		perror("sem_post : ProdCons.request_e_empty");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_receipt_r_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER RECIBO
	//
	// so_prodcons_receipt_r_produce_begin();
    if (sem_wait(ProdCons.receipts_r_empty) == -1)
	{
		perror("sem_wait : ProdCons.receipts_r_empty");
		exit(20);
	}
	
	if (sem_wait(ProdCons.receipts_r_mutex) == -1)
	{
		perror("sem_wait : ProdCons.receipts_r_mutex");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_receipt_r_produce_end() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER RECIBO
	//
	//so_prodcons_receipt_r_produce_end();
    if (sem_post(ProdCons.receipts_r_mutex) == -1)
	{
		perror("sem_post : ProdCons.receipts_r_mutex");
		exit(20);
	}
	
	if (sem_post(ProdCons.receipts_r_full) == -1)
	{
		perror("sem_post : ProdCons.receipts_r_full");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_receipt_r_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RECIBO
	//
	// so_prodcons_receipt_r_consume_begin();
    if (sem_wait(ProdCons.receipts_r_full) == -1)
	{
		perror("sem_wait : ProdCons.receipts_r_full");
		exit(20);
	}
	
	if (sem_wait(ProdCons.receipts_r_mutex) == -1)
	{
		perror("sem_wait : ProdCons.receipts_r_mutex");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_receipt_r_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RECIBO
	//
	//so_prodcons_receipt_r_consume_end();
    if (sem_post(ProdCons.receipts_r_mutex) == -1)
	{
		perror("sem_post : ProdCons.receipts_r_mutex");
		exit(20);
	}
	
	if (sem_post(ProdCons.receipts_r_empty) == -1)
	{
		perror("sem_post : ProdCons.receipts_r_empty");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_buffers_begin() {
	//==============================================
	// GARANTIR EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	// so_prodcons_buffers_begin();
    if (sem_wait(ProdCons.request_p_mutex) == -1)
	{
		perror("sem_wait : ProdCons.request_p_mutex");
		exit(20);
	}
	if (sem_wait(ProdCons.request_e_mutex) == -1)
	{
		perror("sem_wait : ProdCons.request_e_mutex");
		exit(20);
	}
	if (sem_wait(ProdCons.receipts_r_mutex) == -1)
	{
		perror("sem_wait : ProdCons.receipts_r_mutex");
		exit(20);
	}
	//==============================================
}

//******************************************
void prodcons_buffers_end() {
	//==============================================
	// FIM DA ZONA DE EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	// so_prodcons_buffers_end();
    if (sem_post(ProdCons.request_p_mutex) == -1)
	{
		perror("sem_post : ProdCons.request_p_mutex");
		exit(20);
	}
	if (sem_post(ProdCons.request_e_mutex) == -1)
	{
		perror("sem_post : ProdCons.request_e_mutex");
		exit(20);
	}
	if (sem_post(ProdCons.receipts_r_mutex) == -1)
	{
		perror("sem_post : ProdCons.receipts_r_mutex");
		exit(20);
	}
	//==============================================
}

//******************************************
int prodcons_update_stock(int currency) {
	//==============================================
	// OBTER MUTEX DO STOCK E ATUALIZAR STOCK
	//
	// se stock de currency>0 então decrementá-lo de uma unidade e 
	//   função devolve 1
	// se stock de currency=0 então função devolve 0
	//return so_prodcons_update_stock(currency);
    int res=0;
       
    if(sem_wait (ProdCons.stock_mutex)==-1){
        perror("sem_wait : ProdCons.stock_mutex");
        exit(20);
    }
    
    if(Config.stock[currency]>0){
        Config.stock[currency]--;
        res=1;
    }
    
    
    if(sem_post (ProdCons.stock_mutex)==-1){
        perror("sem_post : ProdCons.stock_mutex");
        exit(20);
    }
	return res;
	//==============================================
}

