#ifndef SCHEDULER_H_GUARD
#define SCHEDULER_H_GUARD

// estrutura onde se encontram quais as exchanges que podem fornecer cada currency (1 ou 0)
struct scheduler {
	int *ptr; // ponteiro para matriz [CURRENCIES,EXCHANGES]
};

void scheduler_begin(int, int);
int scheduler_get_exchange(int, int);

#endif
