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
#include <ctype.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "memory.h"
#include "scheduler.h"
#include "file.h"
#include "prodcons.h"
#include "sotime.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct file Ficheiros; // informação sobre nomes e handles de ficheiros

void file_begin(char *fic_in, char *fic_out, char *fic_log) {
	//==============================================
	// GUARDAR NOMES DOS FICHEIROS NA ESTRUTURA Ficheiros
	//
	so_file_begin(fic_in, fic_out, fic_log);
	//==============================================

	//==============================================
	// ABRIR FICHEIRO DE ENTRADA
	// em modo de leitura
	so_file_open_file_in();
	//==============================================

	// parse do ficheiro de teste
	// esta funcao prepara os campos da estrutura Config (char *)
	// com os dados do ficheiro de entrada
	if (ini_parse_file(Ficheiros.h_in, handler, &Config) < 0) {
		printf("Erro a carregar o ficheiro de teste'\n");
		exit(1);
	}

	// agora e' preciso inicializar os restantes campos da estrutura Config

	//==============================================
	// CONTAR CURRENCIES
	// usar strtok para percorrer Config.list_currencies
	// guardar resultado em Config.CURRENCIES
	so_file_count_currencies();
	//==============================================

	// iniciar memoria para o vetor com o stock por CURRENCY e semaforo
	memory_create_stock();
	prodcons_create_stock();

	//==============================================
	// LER STOCK DE CADO CURRENCY
	// percorrer Config.list_currencies e
	// guardar cada valor no vetor Config.stock
	so_file_read_stock();
	//==============================================

	//==============================================
	// CONTAR INVESTORS
	// usar strtok para percorrer Config.list_investors
	// guardar resultado em Config.INVESTORS
	so_file_count_investors();
	//==============================================

	//==============================================
	// CONTAR BROKERS
	// usar strtok para percorrer Config.list_brokers
	// guardar resultado em Config.BROKERS
	so_file_count_brokers();
	//==============================================

	//==============================================
	// CONTAR EXCHANGES
	// usar strtok para percorrer Config.list_exchanges
	// guardar resultado em Config.EXCHANGES
	so_file_count_exchanges();
	//==============================================

	so_file_read_currencies();

	//==============================================
	// LER CAPACITYS DOS BUFFERS
	// usar strtok para percorrer Config.list_buffers
	// guardar primeiro tamanho em Config.BUFFER_REQUEST
	// guardar segundo tamanho em Config.BUFFER_ORDER
	// guardar terceiro tamanho em Config.BUFFER_RECEIPT
	so_file_read_capacities();
	//==============================================

	//==============================================
	// ABRIR FICHEIRO DE SAIDA (se foi especificado)
	// em modo de escrita
	so_file_open_file_out();
	//==============================================

	//==============================================
	// ABRIR FICHEIRO DE LOG (se foi especificado)
	// em modo de escrita
	so_file_open_file_log();
	//==============================================
}

void file_destroy() {
	//==============================================
	// LIBERTAR ZONAS DE MEMÓRIA RESERVADAS DINAMICAMENTE
	// que podem ser: Ficheiros.entrada, Ficheiros.saida, Ficheiros.log
	so_file_destroy();
	//==============================================
}

void file_write_log_file(int etapa, int id) {
	double t_diff;

	if (Ficheiros.h_log != NULL) {

		prodcons_buffers_begin();

		// guardar timestamp
		t_diff = time_untilnow();

		//==============================================
		// ESCREVER DADOS NO FICHEIRO DE LOG
		//
		// o log deve seguir escrupulosamente o formato definido
		so_file_write_log_file(etapa, id, t_diff);
		//==============================================

		prodcons_buffers_end();
	}
}

void file_write_line(char * linha) {
	//==============================================
	// ESCREVER UMA LINHA NO FICHEIRO DE SAIDA
	//
	so_file_write_line(linha);
	//==============================================
}

int stricmp(const char *s1, const char *s2) {
	if (s1 == NULL)
		return s2 == NULL ? 0 : -(*s2);
	if (s2 == NULL)
		return *s1;

	char c1, c2;
	while ((c1 = tolower(*s1)) == (c2 = tolower(*s2))) {
		if (*s1 == '\0')
			break;
		++s1;
		++s2;
	}

	return c1 - c2;
}

int handler(void* user, const char* section, const char* name,
		const char* value) {
	struct configuration* pconfig = (struct configuration*) user;

#define MATCH(s, n) stricmp(section, s) == 0 && stricmp(name, n) == 0
	if (MATCH("currencies", "stock")) {
		pconfig->list_currencies = strdup(value);
	} else if (MATCH("investors", "currency")) {
		pconfig->list_investors = strdup(value);
	} else if (MATCH("brokers", "list")) {
		pconfig->list_brokers = strdup(value);
	} else if (MATCH("exchanges", "currencies")) {
		pconfig->list_exchanges = strdup(value);
	} else if (MATCH("buffers", "capacity")) {
		pconfig->list_buffers = strdup(value);
	} else {
		return 0; /* unknown section/name, error */
	}
	return 1;
}

