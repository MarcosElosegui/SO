#include "PRIORIDADES.h"

#ifndef _CPU_h
#define _CPU_h

struct CPU {
    int id;
    int num_cores;
    struct CORE *cores;
};

struct HILO {
    int id;
	int core_id;
    int estado; //1 ocupado 0 libre
    struct PCB *pcb;    
};

struct CORE {
    int id;
    int num_hilos;
    struct HILO *hilos;
};


struct CPUQUEUE {
	int size;
	struct HILO **hilos;
};

struct CPU * crearCPU(int id, int num_cores, int num_hilos);
int mostrar_procesosCPU(struct CPU * cpu);
struct CPUQUEUE * create_CPUQUEUE(struct CPU * cpu);
int mostrar_colaCPU(struct CPUQUEUE  * cola);

int quitarProcesos_PCB_CPU(struct CPU * cpu, struct CPUQUEUE * cola, struct COLA_PRIORIDADES * cola_prioridades );
int agregarProceso_PCB_CPU(struct CPU * cpu, struct CPUQUEUE * cola, struct PCB * pcb);
int desencolar_PCB_CPU(struct CPU * cpu, struct CPUQUEUE * cola);

int actualizar_tiempos_PCBS(struct CPU * cpu, struct CPUQUEUE * cola);



#endif