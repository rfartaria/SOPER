#ifndef CONTROL_H_GUARD
#define CONTROL_H_GUARD

// estrutura de condições para controlar o encerramento do SO_Currency
struct control {
	pthread_mutex_t brokers_mutex;
	pthread_cond_t  brokers_cond;
	pthread_mutex_t exchanges_mutex;
	pthread_cond_t  exchanges_cond;
	// semaforo para o investor informar os brokers de uma encomenda
	sem_t *broker_request;
	// semaforos para a broker informar a exchange de um pedido
	sem_t **exchange_request;
	// semaforos para a exchange informar o investor de um recibo
	sem_t **investor_receipt;
	int open;  // 0: se SO_Currency closed, 1: se SO_Currency open
};

void control_create();
void control_destroy();
void control_lock();
void control_unlock();
void control_waitfor_request();
void control_wakeup_exchanges();
void control_open_socurrency();
void control_close_socurrency();
void cond_create(pthread_cond_t *cond, pthread_mutex_t *mutex);

void control_investor_submit_request(int);
int  control_broker_waitfor_request(int);
void control_broker_submit_request(int);
int  control_exchange_waitfor_request(int);
void control_exchange_submit_receipt(int);
void control_investor_waitfor_receipt(int);

#endif
