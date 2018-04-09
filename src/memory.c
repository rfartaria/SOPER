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
#include "memory.h"
#include "prodcons.h"
#include "control.h"
#include "file.h"
#include "sotime.h"
#include "scheduler.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct receipt_r BReceipt;  // buffer exchange-investor
struct request_e BOrder; // buffer broker-exchange
struct request_p BCurrency; 	// buffer investor-broker

//******************************************
// CRIAR ZONAS DE MEMORIA
//
void * memory_create(char * name, int size) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE MEMÓRIA PARTILHADA
	//
	// usar getuid() para gerar nome unico na forma:
	// sprintf(name_uid,"/%s_%d", name, getuid())
	// usar name_uid em shm_open
	// usar tambem: ftruncate e mmap
	return so_memory_create(name, size);
	//==============================================
}
void memory_create_stock() {
	//==============================================
	// CRIAR ZONA DE MEMÓRIA PARA O STOCK DE CADO CURRENCY
	//
	// utilizar a função genérica memory_create(char *,int)
	// para criar ponteiro que se guarda em Config.stock
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [CURRENCIES] para inteiro
	so_memory_create_stock();
	//==============================================
}
void memory_create_buffers() {
	//==============================================
	// CRIAR ZONAS DE MEMÓRIA PARA OS BUFFERS: PEDIDOS DE CURRENCY, PEDIDOS DE ENCOMENDA e RECIBOS
	//
	// utilizar a função genérica memory_create(char *,int)
	//
	// para criar ponteiro que se guarda em BReceipt.ptr
	// que deve ter capacidade para um vetor unidimensional
	// de inteiros com a dimensao [BUFFER_RECEIPT]
	// para criar ponteiro que se guarda em BReceipt.buffer
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [BUFFER_RECEIPT] para struct currency
	//
	// para criar ponteiro que se guarda em BOrder.ptr
	// que deve ter capacidade para um vetor unidimensional
	// de inteiros com a dimensao [BUFFER_ORDER]
	// para criar ponteiro que se guarda em BOrder.buffer
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [BUFFER_ORDER] para struct currency
	//
	// para criar ponteiro que se guarda em BCurrency.ptr
	// que deve ter capacidade para struct pointers
	// para criar ponteiro que se guarda em BCurrency.buffer
	// que deve ter capacidade para um vetor unidimensional
	// com a dimensao [BUFFER_REQUEST] para struct currency
	so_memory_create_buffers();
	//==============================================
}
void memory_create_scheduler() {
	//==============================================
	// CRIAR ZONA DE MEMÓRIA PARA O MAPA DE ESCALONAMENTO
	//
	// utilizar a função genérica memory_create(char *,int)
	// para criar ponteiro que se guarda em Schedule.ptr
	// que deve ter capacidade para um vetor bidimensional
	// com a dimensao [CURRENCIES,EXCHANGES] para inteiro
	so_memory_create_scheduler();
	//==============================================
}

void memory_destroy(char * name, void * ptr, int size) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE MEMÓRIA PARTILHADA
	//
	// usar getuid() para gerar nome unico na forma:
	// sprintf(name_uid,"/%s_%d", name, getuid())
	// usar name_uid em shm_unlink
	// usar tambem: munmap
	so_memory_destroy(name, ptr, size);
	//==============================================
}

//******************************************
// MEMORIA_DESTRUIR
//
void memory_destroy_all() {
	//==============================================
	// DESTRUIR MAPEAMENTO E NOME DE PÁGINAS DE MEMÓRIA
	//
	// utilizar a função genérica memory_destroy(char *,void *,int)
	so_memory_destroy_all();
	//==============================================
}

//******************************************
// MEMORIA_REQUEST_P_ESCREVE
//
void memory_request_p_write(int id, struct currency *pProduto) {
	prodcons_request_p_produce_begin();

	// registar hora do pedido de CURRENCY
	time_register(&pProduto->time_request);

	//==============================================
	// ESCREVER PEDIDO DE CURRENCY NO BUFFER PEDIDO DE CURRENCIES
	//
	// copiar conteudo relevante da estrutura pProduto para
	// a posicao BCurrency.ptr->in do buffer BCurrency
	// conteudo: investor, id, time_request
	// colocar available = 1 nessa posicao do BCurrency
	// e atualizar BCurrency.ptr->in
	so_memory_request_p_write(id, pProduto);
	//==============================================

	prodcons_request_p_produce_end();

	// informar BROKER de pedido de CURRENCY
	control_investor_submit_request(id);

	// log
	file_write_log_file(1, id);
}
//******************************************
// MEMORIA_REQUEST_P_LE
//
int memory_request_p_read(int id, struct currency *pProduto) {
	// testar se existem investors e se o SO_STORE esta open
	if (control_broker_waitfor_request(id) == 0)
		return 0;

	prodcons_request_p_consume_begin();

	//==============================================
	// LER PEDIDO DE CURRENCY DO BUFFER PEDIDO DE CURRENCIES
	//
	// copiar conteudo relevante da posicao BCurrency.ptr->out 
	// do buffer BCurrency para a estrutura pProduto
	// conteudo: investor, id, time_request
	// colocar available = 0 nessa posicao do BCurrency
	// e atualizar BCurrency.ptr->out
	so_memory_request_p_read(id, pProduto);
	//==============================================

	// testar se existe stock do CURRENCY pedido pelo investor
	if (prodcons_update_stock(pProduto->id) == 0) {
		pProduto->available = 0;
		prodcons_request_p_consume_end();
		return 2;
	} else
		pProduto->available = 1;

	prodcons_request_p_consume_end();

	// log
	file_write_log_file(2, id);

	return 1;
}

//******************************************
// MEMORIA_REQUEST_E_ESCREVE
//
void memory_request_e_write(int id, struct currency *pProduto) {
	int pos, exchange;

	prodcons_request_e_produce_begin();

	// decidir a que exchange se destina
	exchange = scheduler_get_exchange(id, pProduto->id);

	//==============================================
	// ESCREVER PEDIDO NO BUFFER DE PEDIDOS DE ENCOMENDA
	//
	// procurar posicao vazia no buffer BOrder
	// copiar conteudo relevante da estrutura pProduto para
	// a posicao BOrder.ptr->in do buffer BOrder
	// conteudo: investor, id, available, broker, exchange, time_request
	// colocar BOrder.ptr[n] = 1 na posicao respetiva
	pos = so_memory_request_e_write(id, pProduto, exchange);
	//==============================================

	prodcons_request_e_produce_end();

	// informar exchange respetiva de pedido de encomenda
	control_broker_submit_request(exchange);

	// registar hora pedido (encomenda)
	time_register(&BReceipt.buffer[pos].time_order);

	// log
	file_write_log_file(3, id);
}
//******************************************
// MEMORIA_REQUEST_E_LE
//
int memory_request_e_read(int id, struct currency *pProduto) {
	// testar se existem pedidos e se o SO_Currency esta open
	if (control_exchange_waitfor_request(id) == 0)
		return 0;

	prodcons_request_e_consume_begin();

	//==============================================
	// LER PEDIDO DO BUFFER DE PEDIDOS DE ENCOMENDA
	//
	// procurar pedido de encomenda para a exchange no buffer BOrder
	// copiar conteudo relevante da posicao encontrada
	// no buffer BOrder para pProduto
	// conteudo: investor, id, available, broker, time_request, time_order
	// colocar BOrder.ptr[n] = 0 na posicao respetiva
	so_memory_request_e_read(id, pProduto);
	//==============================================

	prodcons_request_e_consume_end();

	// log
	file_write_log_file(4, id);

	return 1;
}

//******************************************
// MEMORIA_RECEIPT_R_ESCREVE
//
void memory_receipt_r_write(int id, struct currency *pProduto) {
	int pos;

	prodcons_receipt_r_produce_begin();

	//==============================================
	// ESCREVER RECIBO NO BUFFER DE RECIBOS
	//
	// procurar posicao vazia no buffer BReceipt
	// copiar conteudo relevante da estrutura pProduto para
	// a posicao BReceipt.ptr->in do buffer BReceipt
	// conteudo: investor, id, available, broker, exchange, time_request, time_order
	// colocar BReceipt.ptr[n] = 1 na posicao respetiva
	pos = so_memory_receipt_r_write(id, pProduto);
	//==============================================

	prodcons_receipt_r_produce_end();

	// informar investor de que o recibo esta pronto
	control_exchange_submit_receipt(pProduto->investor);

	// registar hora pronta (recibo)
	time_register(&BReceipt.buffer[pos].time_receipt);

	// log
	file_write_log_file(5, id);
}
//******************************************
// MEMORIA_RECEIPT_R_LE
//
void memory_receipt_r_read(int id, struct currency *pProduto) {
	// aguardar recibo
	control_investor_waitfor_receipt(pProduto->investor);

	prodcons_receipt_r_consume_begin();

	//==============================================
	// LER RECIBO DO BUFFER DE RECIBOS
	//
	// procurar recibo para o investor no buffer BReceipt
	// copiar conteudo relevante da posicao encontrada
	// no buffer BReceipt para pProduto
	// conteudo: investor, available, broker, exchange, time_request, time_order, time_receipt
	// colocar BReceipt.ptr[n] = 0 na posicao respetiva
	so_memory_receipt_r_read(id, pProduto);
	//==============================================

	prodcons_receipt_r_consume_end();

	// log
	file_write_log_file(6, id);
}

//******************************************
// MEMORIA_CRIAR_INDICADORES
//
void memory_create_statistics() {
	//==============================================
	// CRIAR ZONAS DE MEMÓRIA PARA OS INDICADORES
	//
	// criação dinâmica de memória
	// para cada campo da estrutura indicadores
	so_memory_create_statistics();
	// iniciar indicadores relevantes:
	// - Ind.initial_stock (c/ Config.stock respetivo)
	// - Ind.investors_servedby_brokers (c/ 0)
	// - Ind.investors_servedby_exchanges (c/ 0)
	// - Ind.currencies_getby_investors (c/ 0)
	so_memory_prepare_statistics();
	//==============================================
}

