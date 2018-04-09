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
#include "memory.h"
#include "sotime.h"
#include "exchange.h"

extern struct statistics Ind;
extern struct configuration Config;

int exchange_executar(int id) {
	struct currency Produto;
	int investors_servedby_exchange = 0;

	setbuf(stdout, NULL);

	// Preparar currencies
	while (1) {
		// aguardar pedido da broker
		if (memory_request_e_read(id, &Produto) == 0)
			break;
		// preparar currency e aguardar processamento de encomenda
		time_processing_order();
		// colocar recibo
		memory_receipt_r_write(id, &Produto);
		Ind.currencies_deliveredby_exchanges[Produto.id * Config.EXCHANGES + id]++;
		investors_servedby_exchange++;
	}
	//    printf("EXCHANGE %02d terminou\n",id);
	return investors_servedby_exchange;
}
