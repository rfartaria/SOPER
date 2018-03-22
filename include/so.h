#ifndef SO_H_GUARD
#define SO_H_GUARD

void so_write_statistics();

/* inih -- simple .INI file parser

 inih is released under the New BSD license (see LICENSE.txt). Go to the project
 home page for more info:
 http://code.google.com/p/inih/


 Parse given INI-style file. May have [section]s, name=value pairs
 (whitespace stripped), and comments starting with ';' (semicolon). Section
 is "" if name=value pair parsed before any section heading.

 For each name=value pair parsed, call handler function with given user
 pointer as well as section, name, and value (data only valid for duration
 of handler call). Handler should return nonzero on success, zero on error.

 Returns 0 on success, line number of first error on parse error, or -1 on
 file open error.
 */
int ini_parse(const char* filename,
		int (*handler)(void* user, const char* section, const char* name,
				const char* value), void* user);
int ini_parse_file(FILE* file,
		int (*handler)(void*, const char*, const char*, const char*),
		void* user);

/* Nonzero to allow multi-line value parsing, in the style of Python's
 ConfigParser. If allowed, ini_parse() will call the handler with the same
 name for each subsequent line parsed. */
#ifndef INI_ALLOW_MULTILINE
#define INI_ALLOW_MULTILINE 1
#endif

//***********************************
// Funções de apoio ao desenvolvimento do trabalho
//***********************************
// MAIN
//
void so_main_broker(int);
void so_main_exchange(int);
void so_main_investor(int);
long so_main_args(int, char**, char **, char **, char **);
void so_main_wait_investors();
void so_main_wait_brokers();
void so_main_wait_exchanges();
//***********************************
// ESCALONADOR
//
void so_scheduler_begin(int, int);
int so_scheduler_get_exchange(int, int);
//***********************************
// MEMORIA
//
void *so_memory_create(char *, int);
void so_memory_create_stock();
void so_memory_create_buffers();
void so_memory_create_scheduler();
void so_memory_destroy(char *, void *, int);
void so_memory_destroy_all();
void so_memory_request_p_write(int, struct currency *);
void so_memory_request_p_read(int, struct currency *);
int  so_memory_request_e_write(int, struct currency *, int);
void so_memory_request_e_read(int, struct currency *);
int  so_memory_receipt_r_write(int, struct currency *);
void so_memory_receipt_r_read(int, struct currency *);
void so_memory_create_statistics();
void so_memory_prepare_statistics();
//***********************************
// PRODCONS
//
sem_t *so_semaphore_create(char *, int);
void so_prodcons_create_stock();
void so_prodcons_create_buffers();
void so_semaphore_destroy(char *, sem_t *);
void so_prodcons_destroy();
void so_prodcons_request_p_produce_begin();
void so_prodcons_request_p_produce_end();
void so_prodcons_request_p_consume_begin();
void so_prodcons_request_p_consume_end();
void so_prodcons_request_e_produce_begin();
void so_prodcons_request_e_produce_end();
void so_prodcons_request_e_consume_begin();
void so_prodcons_request_e_consume_end();
void so_prodcons_receipt_r_produce_begin();
void so_prodcons_receipt_r_produce_end();
void so_prodcons_receipt_r_consume_begin();
void so_prodcons_receipt_r_consume_end();
void so_prodcons_buffers_begin();
void so_prodcons_buffers_end();
int  so_prodcons_update_stock(int);
//***********************************
// CONTROLO
//
void so_cond_create(pthread_cond_t *, pthread_mutex_t *);
void so_control_create();
void so_control_create_opening();
void so_control_create_wakeup_investor();
void so_control_create_wakeup_exchange();
void so_control_create_wakeup_broker();
void so_cond_destroy(pthread_cond_t *, pthread_mutex_t *);
void so_control_destroy();
void so_control_destroy_opening();
void so_control_destroy_wakeup_investor();
void so_control_destroy_wakeup_exchange();
void so_control_destroy_wakeup_broker();
void so_control_open_socurrency();
void so_control_close_socurrency();
void so_control_investor_submit_request(int);
int  so_control_broker_waitfor_request(int);
void so_control_broker_submit_request(int);
int  so_control_exchange_waitfor_request(int);
void so_control_exchange_submit_receipt(int);
void so_control_investor_waitfor_receipt(int);
//***********************************
// FICHEIRO
//
void so_file_begin(char *, char *, char *);
void so_file_destroy();
void so_file_open_file_in();
void so_file_count_currencies();
void so_file_read_stock();
void so_file_count_investors();
void so_file_count_brokers();
void so_file_count_exchanges();
void so_file_read_currencies();
void so_file_read_capacities();
void so_file_open_file_out();
void so_file_open_file_log();
void so_file_write_log_file(int, int, double);
void so_file_write_line(char *);
//***********************************
// TEMPO
//
void so_time_begin(long);
void so_time_setup_alarm();
void so_time_destroy(long);
void so_time_write_log_timed(int);
double so_time_difference(struct timespec, struct timespec);
double so_time_untilnow();
void so_time_register(struct timespec *);
void so_time_processing_order();

#endif
