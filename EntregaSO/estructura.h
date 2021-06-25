#ifndef _estructura_h
#define _estructura_h

struct generador_procesos{
	int id_gen;
	pthread_t pthid;
};

struct clock{
	int id_clock;
	pthread_t pthid;
};

struct temporizador{
	int id_temp;
	pthread_t pthid;
	
};

struct scheduler{
	int id_sch;
	pthread_t pthid;
};

void * generador_Procesos(void *id_gen);

#endif