#ifndef MAIN_H_GUARD
#define MAIN_H_GUARD

// estrutura que representa uma currency nas tres etapas: pedido da currency, pedido de encomenda e recibo
struct currency {
	int id;  		            	// identificador do tipo de currency pedido
	int available;                 	// stock: 0 - unavailable, 1 - available
	int investor;     	            // id do investor que requested a currency
	int broker;    	        		// id da broker que atendeu o investor
	int exchange;  	            	// id da exchange que efetuou a currency
	struct timespec time_request; 	// hora a que a currency foi pedido a broker (registada pelo investor)
	struct timespec time_order;  	// hora a que a encomenda foi pedida a exchange (registada pela broker)
	struct timespec time_receipt; 	// hora a que o recibo foi produzido pela exchange (registada pela exchange)
};

// estrutura com valores dos parâmetros do ficheiro de teste
struct configuration {
	char* list_currencies;      	// linha com stock de cada currency
	char* list_investors;    		// linha com produp pretendido por cada investor
	char* list_brokers; 			// linha com os brokers (nomes nao interessam)
	char* list_exchanges; 			// linha com os currencies por exchange separadas por virgula
	char* list_buffers;     		// linha com capacidade dos tres buffers
	int CURRENCIES;            		// nº de currencies disponiveis
	int INVESTORS;          		// nº de investors
	int BROKERS;        			// nº de brokers
	int EXCHANGES;       			// nº de exchanges
	int BUFFER_REQUEST; 			// capacidade do buffer de pedidos de currency
	int BUFFER_ORDER;    			// capacidade do buffer de pedidos de encomenda
	int BUFFER_RECEIPT;     		// capacidade do buffer de recibos
	int* stock;             		// vetor com capacidade por tipo de currency
};

// estrutura de registo de indicadores de funcionamento do SO_Currency
struct statistics {
	int *initial_stock;
	int *pid_investors;
	int *pid_brokers;
	int *pid_exchanges;
	int *investors_servedby_brokers;
	int *investors_servedby_exchanges;
	int *currencies_deliveredby_exchanges;
	int *currencies_getby_investors;
};

#endif
