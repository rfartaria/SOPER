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
#include "investor.h"
#include "exchange.h"
#include "broker.h"

struct statistics Ind;     // indicadores do funcionamento do SO_Currency
struct configuration Config; // configuração da execução do SO_Currency

/* main_investor recebe como parâmetro o nº de investors a criar */
/* main_investor receives the number of investors to be created as a parameter */
void main_investor(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos investor igual a quant através de um ciclo com n=0,1,...
	// após a criação de cada processo, o processo filho realiza duas atividades:
	// - chama a função investor_executar passando como parâmetro o valor da variável de controlo do ciclo n=0,1,...
	// - chama a função exit passando como parâmetro o valor devolvido pela função investor_executar
	// o processo pai guarda o pid do filho no vetor Ind.pid_investors[n], com n=0,1,... e termina normalmente a função
    //==============================================
    // CREATE PROCESSES
    //
    // create a number of investor processes equal to "quant", each one will have an ID of 0,1,...
    // after creating each process, the child process does two tasks:
	// - calls the function investor_executar passing one parameter that is the ID (0,1,...)
    // - calls the exit function passing  as parameter the value returned by the function investor_executar
	// the parent process stores the PID of the child process in the array Ind.pid_investors[n], using the ID=0,1,... and returns from the function
    so_main_investor(quant);
    //==============================================
}

/* main_broker recebe como parâmetro o nº de brokers a criar */
/* main_broker receives the number of brokers to be created as a parameter */
void main_broker(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos investor igual a quant através de um ciclo com n=0,1,...
	// após a criação de cada processo, o processo filho realiza duas atividades:
	// - chama a função investor_executar passando como parâmetro o valor da variável de controlo do ciclo n=0,1,...
	// - chama a função exit passando como parâmetro o valor devolvido pela função broker_executar
	// o processo pai guarda o pid do filho no vetor Ind.pid_brokers[n], com n=0,1,... e termina normalmente a função
    //==============================================
    // CREATE PROCESSES
    //
    // create a number of broker processes equal to "quant", each one will have an ID of 0,1,...
    // after creating each process, the child process does two tasks:
	// - calls the function broker_executar passing one parameter that is the ID (0,1,...)
    // - calls the exit function passing  as parameter the value returned by the function broker_executar
	// the parent process stores the PID of the child process in the array Ind.pid_brokers[n], using the ID=0,1,... and returns from the function
    so_main_broker(quant);
    //==============================================
}

/* main_exchange recebe como parâmetro o nº de exchanges a criar */
/* main_exchange receives the number of exchanges to be created as a parameter */
void main_exchange(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos investor igual a quant através de um ciclo com n=0,1,...
	// após a criação de cada processo, o processo filho realiza duas atividades:
	// - chama a função investor_executar passando como parâmetro o valor da variável de controlo do ciclo n=0,1,...
	// - chama a função exit passando como parâmetro o valor devolvido pela função exchange_executar
	// o processo pai guarda o pid do filho no vetor Ind.pid_exchanges[n], com n=0,1,... e termina normalmente a função
    //==============================================
    // CREATE PROCESSES
    //
    // create a number of exchange processes equal to "quant", each one will have an ID of 0,1,...
    // after creating each process, the child process does two tasks:
	// - calls the function exchange_executar passing one parameter that is the ID (0,1,...)
    // - calls the exit function passing  as parameter the value returned by the function exchange_executar
	// the parent process stores the PID of the child process in the array Ind.pid_exchange[n], using the ID=0,1,... and returns from the function
    so_main_exchange(quant);
    //==============================================
}

int main(int argc, char *argv[])
{
    char *ficEntrada = NULL;
    char *ficSaida = NULL;
    char *ficLog = NULL;
    long intervalo = 0;

    //==============================================
    // TRATAR PARÂMETROS DE ENTRADA
    // parâmetro obrigatório: file_configuracao
    // parâmetros opcionais:
    //   file_resultados
    //   -l file_log
    //   -t intervalo(us)    // us: microsegundos
    //
	// ignorar parâmetros desconhecidos
	// em caso de ausência de parâmetros escrever mensagem de como utilizar o programa e terminar
	// considerar que os parâmetros apenas são introduzidos na ordem indicada pela mensagem
	// considerar que são sempre introduzidos valores válidos para os parâmetros
    //==============================================
    // PROCESS INPUT PARAMETERS
    // required parameter: file_configuration
    // optional parameters:
    //   file_results
    //   -l file_log
    //   -t interval(us)    // us: microseconds
    //
	// ignore unknow parameters
	// if no parameters are introduced, write a message to the console informing about the correct usage
	// consider that the parameters are always introduced in the same order
	// consider that the values introduced as parameters are always valid
    intervalo = so_main_args(argc, argv, &ficEntrada, &ficSaida, &ficLog);
    //==============================================

    printf(
	"\n------------------------------------------------------------------------");
    printf(
	"\n----------------------------- SO_Currency ------------------------------");
    printf(
	"\n------------------------------------------------------------------------\n");

    // Ler dados de entrada
    file_begin(ficEntrada, ficSaida, ficLog);
    // criar zonas de memória e semáforos
    memory_create_buffers();
    prodcons_create_buffers();
    control_create();

    // Criar estruturas para indicadores e guardar stock inicial
    memory_create_statistics();

    printf("\n\t\t\t*** Open SO_Currency ***\n\n");
    control_open_socurrency();

    // Registar início de operação e armar alarme
    time_begin(intervalo);

    //
    // Iniciar sistema
    //

    // Criar BROKERS
    main_broker(Config.BROKERS);
    // Criar EXCHANGES
    main_exchange(Config.EXCHANGES);
    // Criar INVESTORS
    main_investor(Config.INVESTORS);

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS INVESTORS E ATUALIZAR ESTATISTICAS
    //
    // esperar por cada investor individualmente
	// se o investor terminou normalmente 
	// então testar se o valor do status é igual a CURRENCIES
    //   se for igual entao nao atualizar as estatisticas 
    //   se for inferior entao incrementar o indicador da respetiva currency obtida
    // Ind.currencies_getby_investors[CURRENCY], n=0,1,...
    //==============================================
    // WAIT FOR THE INVESTOR PROCESSES TERMINATION AND UPDATE STATISTICS
    //
    // wait for each investor process
	// if the investor has terminated normally
    // then test if the value of status is equal to CURRENCIES
    //   if it is equal then don't update statistics
    //   if it is less than, increment the respective currency
	// then increment the statistical data relative to the number of currencies obtained by investors
    // Ind.currencies_getby_investors[CURRENCY], n=0,1,...
    so_main_wait_investors();
    //==============================================

    // Desarmar alarme
    time_destroy(intervalo);

    printf("\n\t\t\t*** Close SO_Currency ***\n\n");
    control_close_socurrency();

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS BROKERS E ATUALIZAR INDICADORES
    //
    // esperar por cada broker individualmente
	// se a broker terminou normalmente 
	// então atualizar o indicador de investors atendidos
    // Ind.investors_servedby_brokers[n], n=0,1,...
    // guardando nele o estado devolvido pela terminação do processo
    //==============================================
    // WAIT FOR THE BROKERS PROCESSES TERMINATION AND UPDATE STATISTICS
    //
    // wait for each broker process
	// if the broker has terminated normally
	// then update the statistical data relative to the number of investors that have been served by brokers
    // Ind.investors_servedby_brokers[n], n=0,1,...
    // storing in it the status returned upon the child process termination
    so_main_wait_brokers();
    //==============================================

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DAS EXCHANGES E ATUALIZAR INDICADORES
    //
    // esperar por cada exchange individualmente
	// se a exchange terminou normalmente 
	// então atualizar o indicador de investors atendidos
    // Ind.investors_servedby_exchanges[n], n=0,1,...
    // guardando nele o estado devolvido pela terminação do processo
    //==============================================
    // WAIT FOR THE EXCHANGE PROCESSES TERMINATION AND UPDATE STATISTICS
    //
    // wait for each exchange process
	// if the exchange has terminated normally
	// then update the statistical data relative to the number of investors that have been served by exchanges
    // Ind.investors_servedby_exchanges[n], n=0,1,...
    // storing in it the status returned upon the child process termination
    so_main_wait_exchanges();
    //==============================================

    printf(
	"------------------------------------------------------------------------\n\n");
    printf("\t\t\t*** Statistics ***\n\n");
    so_write_statistics();
    printf("\t\t\t*******************\n");

    // destruir zonas de memória e semáforos
    file_destroy();
    control_destroy();
    prodcons_destroy();
    memory_destroy_all();

    return 0;
}
