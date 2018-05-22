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
//sem_t * semaphore_create(char * name, int value) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE UM SEMÁFORO
	//
	//return so_semaphore_create(name, value);
	//==============================================
//}

void prodcons_create_stock() {
	//==============================================
	// CRIAR MUTEX PARA CONTROLAR O ACESSO AO STOCK
	//
	// utilizar a função genérica semaphore_create
	so_prodcons_create_stock();
	//==============================================
}

void prodcons_create_buffers() {
	//==============================================
	// CRIAR SEMAFOROS PARA CONTROLAR O ACESSO AOS 3 BUFFERS
	//
	// utilizar a função genérica semaphore_create
	so_prodcons_create_buffers();
	//==============================================
}

//void semaphore_destroy(char * name, void * ptr) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE UM SEMÁFORO
	//
//	so_semaphore_destroy(name, ptr);
	//==============================================
//}

void prodcons_destroy() {
	//==============================================
	// DESTRUIR SEMÁFORO E RESPETIVO NOME
	//
	// utilizar a função genérica semaphore_destroy
	so_prodcons_destroy();
	//==============================================
}

//******************************************
void prodcons_request_p_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER PEDIDO DE CURRENCY
	//
	so_prodcons_request_p_produce_begin();
	//==============================================
}

//******************************************
void prodcons_request_p_produce_end() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER PEDIDO DE CURRENCY
	//
	so_prodcons_request_p_produce_end();
	//==============================================
}

//******************************************
void prodcons_request_p_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE CURRENCY
	//
	so_prodcons_request_p_consume_begin();
	//==============================================
}

//******************************************
void prodcons_request_p_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE CURRENCY
	//
	so_prodcons_request_p_consume_end();
	//==============================================
}

//******************************************
void prodcons_request_e_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER PEDIDO DE ENCOMENDA
	//
	so_prodcons_request_e_produce_begin();
	//==============================================
}

//******************************************
void prodcons_request_e_produce_end() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER PEDIDO DE ENCOMENDA
	//
	so_prodcons_request_e_produce_end();
	//==============================================
}

//******************************************
void prodcons_request_e_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE ENCOMENDA
	//
	so_prodcons_request_e_consume_begin();
	//==============================================
}

//******************************************
void prodcons_request_e_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE ENCOMENDA
	//
	so_prodcons_request_e_consume_end();
	//==============================================
}

//******************************************
void prodcons_receipt_r_produce_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER RECIBO
	//
	so_prodcons_receipt_r_produce_begin();
	//==============================================
}

//******************************************
void prodcons_receipt_r_produce_end() {
	//==============================================
	// CONTROLAR ACESSO DE CURRENCYR AO BUFFER RECIBO
	//
	so_prodcons_receipt_r_produce_end();
	//==============================================
}

//******************************************
void prodcons_receipt_r_consume_begin() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RECIBO
	//
	so_prodcons_receipt_r_consume_begin();
	//==============================================
}

//******************************************
void prodcons_receipt_r_consume_end() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RECIBO
	//
	so_prodcons_receipt_r_consume_end();
	//==============================================
}

//******************************************
void prodcons_buffers_begin() {
	//==============================================
	// GARANTIR EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	so_prodcons_buffers_begin();
	//==============================================
}

//******************************************
void prodcons_buffers_end() {
	//==============================================
	// FIM DA ZONA DE EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	so_prodcons_buffers_end();
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
	return so_prodcons_update_stock(currency);
	//==============================================
}
