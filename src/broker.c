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
#include "broker.h"

int broker_executar(int id) {
	int ret;
	struct currency Produto;
	int investors_served_broker = 0;

	setbuf(stdout, NULL);

	// Passar pedidos de currencies para a exchange
	while (1) {
		ret = memory_request_p_read(id, &Produto);
		if (ret == 1) {
			// currency disponível
			memory_request_e_write(id, &Produto);
		} else if (ret == 2) {
			// currency indisponível
			memory_receipt_r_write(id, &Produto);
		} else
			// sem investors
			break;
		investors_served_broker++;
	}

	return investors_served_broker;
}
