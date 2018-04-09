#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "scheduler.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct scheduler Schedule; // estrutura de suporte às decisões de scheduling

void scheduler_begin(int exchange, int currency) {
	//==============================================
	// REGISTAR QUE A EXCHANGE FORNECE ESTE CURRENCY
	//
	// escrever 1 na posição respetiva do vetor Schedule.ptr
	so_scheduler_begin(exchange, currency);
	//==============================================
}

int scheduler_get_exchange(int broker, int currency) {
	//==============================================
	// ESCOLHER EXCHANGE PARA ENCOMENDA O CURRENCY
	//
	// dada uma currency, escolher uma das exchanges que disponibilize a currency
	//
	// a informação do broker que está a pedir uma exchange
	// pode não ser necessária
	return so_scheduler_get_exchange(broker, currency);
	//==============================================
}
