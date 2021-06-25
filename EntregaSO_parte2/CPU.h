#include "PRIORIDADES.h"
#include "memoria.h"

#ifndef _CPU_h
#define _CPU_h

struct HILO {
    int id;
	int core_id;
    int estado; //1ocupado 0 libre
    struct PCB *pcb;   
    //int PC;
    //int PTBR;
    int * registros;
};

struct CORE {
    int id;
    int num_hilos;
    struct HILO *hilos;
};

struct CPU {
    int id;
    int num_cores;
    struct CORE *cores;
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

int ejecutar_instruccion(struct CPU * cpu, struct CPUQUEUE * cola, struct memoria * memoria);
void leer_instruccion(struct memoria * memoria, struct PCB * pcb,struct HILO * hilo);



#endif