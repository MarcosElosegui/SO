#ifndef _PRIORIDADES_h
#define _PRIORIDADES_h


struct COLA_PROCESOS{ //PRIORIDAD
	int num_procesos;
	int size;
	struct PCB * pcbs;
	int prioridad;	
	int pos_actual;
	int ultimo;
	
};

struct COLA_PRIORIDADES{
	struct COLA_PROCESOS * procesos;
	int size;	
};

struct COLA_PRIORIDADES * crear_cola_prioridades(int num_procesos, int prioridad);
int encolar_proceso(struct COLA_PRIORIDADES * cola , struct PCB * pcb);
int mostrar_cola_prioridades(struct COLA_PRIORIDADES * cola);
struct PCB * desencolar_primer_proceso(struct COLA_PRIORIDADES * cola);

#endif
