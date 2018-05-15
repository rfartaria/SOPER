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
#include "prodcons.h"
#include "file.h"
#include "sotime.h"
#include "scheduler.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuration Config;
//==============================================

struct file Ficheiros; // informação sobre nomes e handles de ficheiros

void file_begin(char *fic_in, char *fic_out, char *fic_log) {
	//==============================================
    // SAVE FILE NAMES IN STRUCT Ficheiros
	//
	//so_file_begin(fic_in, fic_out, fic_log);
    Ficheiros.entrada = (char *) calloc(1, sizeof(fic_in)+1);
    strcpy(Ficheiros.entrada, fic_in);

	Ficheiros.saida=(char *) calloc(1,sizeof(fic_out)+1);
	strcpy(Ficheiros.saida, fic_out);

	Ficheiros.log=(char *) calloc(1,sizeof(fic_log)+1);
	strcpy(Ficheiros.log, fic_log);
	//==============================================

	//==============================================
    // OPEN INPUT FILE
	// read mode
    // so_file_open_file_in();
    Ficheiros.h_in = fopen(Ficheiros.entrada,"r");
	//==============================================

    // parse test file
    // this function prepares the fields of struct Config (char*)
	// with data from input file
	if (ini_parse_file(Ficheiros.h_in, handler, &Config) < 0) {
		printf("Erro a carregar o ficheiro de teste'\n");
		exit(1);
	}

    // now we need to initialize the remaining fields in struct Config

	//==============================================
	// COUNT CURRENCIES
    // use strtok to iterate Config.list_currencies
    // save the result in Config.CURRENCIES
	// so_file_count_currencies();
    //printf("%s", Config.list_currencies); //exit(0);
    char tmpstr[1000] = {0};
	strcpy(tmpstr,Config.list_currencies);
    char *cch;
    cch = strtok(tmpstr," ");
	Config.CURRENCIES=0;
    while (cch != NULL)
    {
		Config.CURRENCIES++;
		cch = strtok (NULL, " ");
    }
    //printf("\n%d\n", Config.CURRENCIES); exit(0);
	//==============================================

    // initialize memory for vector containing the stock for CURRENCY and semaphore
	memory_create_stock();
	prodcons_create_stock();

	//==============================================
    // READ STOCK OF EACH CURRENCY
    // iterate Config.list_currencies and
    // save each value in the array Config.stock
    // so_file_read_stock();
    //printf("\nstr = %s\n", Config.list_currencies);
    strcpy(tmpstr,Config.list_currencies);
	cch = strtok(tmpstr," ");
	int *istock = Config.stock;
    int count = 0;
    while (cch != NULL)
    {
		sscanf(cch, "%d", istock);
		cch = strtok (NULL, " ");
		istock++;
        //printf("\n%d\n", Config.stock[count]);
        count++;
    }
    //exit(0);
	//==============================================

	//==============================================
	// COUNT INVESTORS
    // use strtok to iterate Config.list_investors
	// keep the result in  Config.INVESTORS
	so_file_count_investors();
//    printf("\n%s\n", Config.list_investors); //exit(0);
//    strcpy(tmpstr,Config.list_investors);
//    cch = strtok(tmpstr," ");
//	Config.INVESTORS = 0;
//    while (cch != NULL)
//    {
//		Config.INVESTORS++;
//        cch = strtok (NULL, " ");
//    }
//    printf("\n%d\n", Config.INVESTORS); //exit(0);
	//==============================================

	//==============================================
	// COUNT BROKERS
    // use strtok to iterate Config.list_brokers
	// keep the result in Config.BROKERS
	//so_file_count_brokers();
    //printf("\n%s\n", Config.list_brokers); exit(0);
    strcpy(tmpstr,Config.list_brokers);
    cch = strtok(tmpstr," ");
	Config.BROKERS = 0;
    while (cch != NULL)
    {
		Config.BROKERS++;
        cch = strtok (NULL, " ");
    }
	//==============================================

	//==============================================
	// COUNT EXCHANGES
	// usar strtok para percorrer Config.list_exchanges
	// guardar resultado em Config.EXCHANGES
	so_file_count_exchanges();
    //printf("\n%s\n", Config.list_exchanges); exit(0);
    strcpy(tmpstr,Config.list_exchanges);
    cch = strtok(tmpstr,",");
	Config.EXCHANGES = 0;
    while (cch != NULL)
    {
		Config.EXCHANGES++;
        cch = strtok (NULL, ",");
    }
	//==============================================

	so_file_read_currencies();

	//==============================================
    // READ CAPACITIES OF BUFFERS
    // use strtok to iterate Config.list_fuffers
    // save first size in Config.BUFFER_REQUEST
    // save second size in Config.BUFFER_ORDER
    // save third size in Config.BUFFER_RECEIPT
	//so_file_read_capacities();
    strcpy(tmpstr,Config.list_buffers);
    cch = strtok(tmpstr," ");
    sscanf(cch, "%d", &Config.BUFFER_REQUEST);
    
    cch = strtok(NULL," ");
    sscanf(cch, "%d", &Config.BUFFER_ORDER);
    
    cch = strtok(NULL," ");
    sscanf(cch, "%d", &Config.BUFFER_RECEIPT);	
	//==============================================

	//==============================================
    // OPEN OUTPUT FILE (if given)
    // write mode
	// so_file_open_file_out();
    if (Ficheiros.saida != NULL)
    {
        Ficheiros.h_out = fopen(Ficheiros.saida,"w");
    }
	//==============================================

	//==============================================
	// ABRIR FICHEIRO DE LOG (se foi especificado)
    // OPEN LOG FILE (if given)
	// write mode
	// so_file_open_file_log();
    if (Ficheiros.log != NULL)
    {
        Ficheiros.h_log = fopen(Ficheiros.log,"w");
    }
	//==============================================
}

void file_destroy() {
	//==============================================
    // FREE DYNAMICALLY ALLOCATED MEMORY
    // those may be: Ficheiros.entrada, Ficheiros.saida, Ficheiros.log
	// so_file_destroy();
    free(Ficheiros.entrada);
    free(Ficheiros.saida);
    free(Ficheiros.log);
    if (ftell(Ficheiros.h_in) >= 0) fclose(Ficheiros.h_in);
    if (ftell(Ficheiros.h_out) >= 0) fclose(Ficheiros.h_out);
    if (ftell(Ficheiros.h_log) >= 0) fclose(Ficheiros.h_log);
	//==============================================
}

void file_write_log_file(int etapa, int id) {
	double t_diff;

	if (Ficheiros.h_log != NULL) {

		prodcons_buffers_begin();

		// guardar timestamp
		t_diff = time_untilnow();

		//==============================================
        // WRITE DATA IN LOG FILE
		//
        // the log must scrupulously follow the defined format
		so_file_write_log_file(etapa, id, t_diff);
		//==============================================

		prodcons_buffers_end();
	}
}

void file_write_line(char * linha) {
	//==============================================
	// ESCREVER UMA LINHA NO FICHEIRO DE SAIDA
	//
	// so_file_write_line(linha);
    fprintf(Ficheiros.h_out, "%s", linha);
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

