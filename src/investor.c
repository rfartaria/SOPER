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
#include "file.h"
#include "sotime.h"
#include "investor.h"

extern struct configuration Config;

int investor_executar(int id) {
	int n, count, ret;
	struct currency Produto;
	int currency_id;
	char buf[100];
	char *result;

	setbuf(stdout, NULL);

	n = 0;
	count = 0;
	result = Config.list_investors;
	while (n < id) {
		while (Config.list_investors[count++] != '\0')
			;
		result = &Config.list_investors[count];
		n++;
	}

	currency_id = atoi(result);
	Produto.id = currency_id;
	Produto.investor = id;

	memory_request_p_write(id, &Produto);
	memory_receipt_r_read(id, &Produto);

	if (Produto.available == 1) {
		printf(
				"     INVESTOR %03d requested %d and got %d (v:%02d c:%02d t:%.9fs)\n",
				id, currency_id, Produto.id, Produto.broker, Produto.exchange,
				time_difference(Produto.time_request, Produto.time_receipt));
		sprintf(buf, "     INVESTOR %03d requested %d and got %d\n", id,
				currency_id, Produto.id);
		ret = Produto.id;
	} else {
		printf("     INVESTOR %03d requested %d but was not available!\n",
				id, currency_id);
		sprintf(buf,
				"     INVESTOR %03d requested %d but was not available!\n",
				id, currency_id);
		ret = Config.CURRENCIES;
	}

	file_write_line(buf);

	return ret;
}
