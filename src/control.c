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

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct control *Controlo;          // controlo da abertura do SO_Currency

void cond_create(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE UMA CONDIÇÃO PARTILHÁVEL ENTRE PROCESSOS
	//
	so_cond_create(cond, mutex);
	//==============================================
}

void control_create() {
	//==============================================
	// CRIAÇÃO DE ZONA DE MEMÓRIA PARTILHADA
	//
	// Criar e iniciar variaveis de condicao e respetivos mutexes do Controlo
	so_control_create();
	//==============================================

	//==============================================
	// CRIAÇÃO DE MECANISMOS DE CONTROLO DE ABERTURA DO SO_Currency
	//
	// Criar e iniciar variaveis de condicao e respetivos mutexes do Controlo
	so_control_create_opening();
	//==============================================

	//==============================================
	// CRIAÇÃO DE MECANISMOS PARA EXCHANGE ACORDAR INVESTOR A QUE SE DESTINA O RECIBO
	//
	// Criar e iniciar semáforos
	so_control_create_wakeup_investor();
	//==============================================

	//==============================================
	// CRIAÇÃO DE MECANISMOS PARA BROKER ACORDAR EXCHANGE A QUE SE DESTINA O PEDIDO DE ENCOMENDA
	//
	// Criar e iniciar semáforos
	so_control_create_wakeup_exchange();
	//==============================================

	//==============================================
	// CRIAÇÃO DE MECANISMOS PARA INVESTOR ACORDAR BROKER A QUE SE DESTINA O PEDIDO DE CURRENCY
	//
	// Criar e iniciar semáforos
	so_control_create_wakeup_broker();
	//==============================================

}

void cond_destroy(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE VARIÁVEIS DE CONDIÇÃO
	//
	so_cond_destroy(cond, mutex);
	//==============================================
}

void control_destroy() {
	//==============================================
	// DESTRUIÇÃO DE MECANISMOS DE CONTROLO DE ABERTURA DO SO_Currency
	//
	// utilizar a função genérica cond_destroy
	so_control_destroy_opening();
	//==============================================

	//==============================================
	// DESTRUIR SEMÁFOROS E RESPETIVOS NOMES
	//
	// utilizar a função genérica prodcons_destroy
	so_control_destroy_wakeup_investor();
	so_control_destroy_wakeup_exchange();
	so_control_destroy_wakeup_broker();
	//==============================================

	//==============================================
	// DESTRUIR ZONA DE MEMÓRIA
	//
	// utilizar a função genérica memory_destroy
	so_control_destroy();
	//==============================================
}

//***************************************************
// ABERTURA DO SO_Currency
//
void control_open_socurrency() {
	//==============================================
	// ABRIR SO_Currency
	//
	so_control_open_socurrency();
	//==============================================
}

void control_close_socurrency() {
	//==============================================
	// FECHAR SO_Currency
	//
	so_control_close_socurrency();
	//==============================================
}

//***************************************************
// CONTROLO ENTRE INVESTORS/BROKERS/EXCHANGES
//
void control_investor_submit_request(int id) {
	//==============================================
	// SINALIZAR BROKER DA SUBMISSÃO DE PEDIDO DE CURRENCY
	//
	so_control_investor_submit_request(id);
	//==============================================
}
//******************************************
int control_broker_waitfor_request(int id) {
	//==============================================
	// BROKER AGUARDA SUBMISSÃO DE PEDIDO DE CURRENCY
	//
	// verificar também se SO_Currency está open
	return so_control_broker_waitfor_request(id);
	//==============================================
}
//******************************************
void control_broker_submit_request(int id) {
	//==============================================
	// SINALIZAR EXCHANGE DA SUBMISSÃO DE PEDIDO DE ENCOMENDA
	//
	so_control_broker_submit_request(id);
	//==============================================
}
//******************************************
int control_exchange_waitfor_request(int id) {
	//==============================================
	// EXCHANGE AGUARDA SUBMISSÃO DE PEDIDO DE ENCOMENDA
	//
	// verificar também se SO_Currency está open
	return so_control_exchange_waitfor_request(id);
	//==============================================
}
//******************************************
void control_exchange_submit_receipt(int id) {
	//==============================================
	// SINALIZAR INVESTOR DA SUBMISSÃO DE RECIBO
	//
	so_control_exchange_submit_receipt(id);
	//==============================================
}
//******************************************
void control_investor_waitfor_receipt(int id) {
	//==============================================
	// INVESTOR AGUARDA SUBMISSÃO DE RECIBO
	//
	so_control_investor_waitfor_receipt(id);
	//==============================================
}
