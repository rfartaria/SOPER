#ifndef MEMORY_H_GUARD
#define MEMORY_H_GUARD

// Nomes usados na criação de zonas de memoria partilhada
#define STR_SHM_STOCK 				"shm_stock"
#define STR_SHM_RECEIPT_PTR 		"shm_receipt_ptr"
#define STR_SHM_RECEIPT_BUFFER 		"shm_receipt_buffer"
#define STR_SHM_ORDER_PTR 			"shm_order_ptr"
#define STR_SHM_ORDER_BUFFER 		"shm_order_buffer"
#define STR_SHM_CURRENCY_PTR 		"shm_currency_ptr"
#define STR_SHM_CURRENCY_BUFFER 	"shm_currency_buffer"
#define STR_SHM_SCHEDULER 			"shm_scheduler"
#define STR_SHM_PRODEXCHANGES		"shm_currencies_exchanges"

// estrutura que contêm os ponteiros para um buffer circular
struct pointers {
	int in;  // ponteiro de escrita
	int out; // ponteiro de leitura
};

// estrutura onde são guardadas os pedidos de currencies do investor para a broker
struct request_p {
	struct currency *buffer;   // ponteiro para a lista de pedidos de currency
	struct pointers *ptr; // ponteiro para a estrutura de índices de escrita e leitura
};

// estrutura onde são guardados os pedidos de encomenda da broker para a exchange
struct request_e {
	struct currency *buffer; // ponteiro para a lista de pedidos de encomenda
	int *ptr;             // ponteiro para a lista de inteiros que indicam
						  // se a posição respetiva está livre ou ocupada
};

// estrutura onde a exchange disponibiliza os recibos
struct receipt_r {
	struct currency *buffer; // ponteiro para a lista de recibos
	int *ptr;             // ponteiro para a lista de inteiros que indicam
						  // se a posição respetiva está livre ou ocupada
};

void * memory_create(char*, int);
void memory_create_stock();
void memory_create_buffers();
void memory_create_scheduler();
void memory_destroy_all();
void memory_request_p_write(int, struct currency *);
int  memory_request_p_read(int, struct currency *);
void memory_request_e_write(int, struct currency *);
int  memory_request_e_read(int, struct currency *);
void memory_receipt_r_write(int, struct currency *);
void memory_receipt_r_read(int, struct currency *);
void memory_create_statistics();
void memory_destroy(char * name, void * ptr, int size);

#endif
