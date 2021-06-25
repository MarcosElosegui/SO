#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "CPU.h"
#include "PCB.h"
#include "PRIORIDADES.h"


/*
	Crea una CPU con la cantidad de cores e hilos especificados.
*/
struct CPU * crearCPU(int id, int num_cores, int num_hilos){
	int i,j;
	
	struct CPU * cpu = (struct CPU *) malloc(sizeof(struct CPU));
	cpu->id = id;
	cpu->num_cores =num_cores;
	cpu->cores = (struct CORE *) malloc (sizeof(struct CORE)*num_cores);
	
	for(i=0;i<num_cores;i++){
		struct CORE * core = (struct CORE *)malloc(sizeof(struct CORE));
		core->id = i;
		core->num_hilos = num_hilos;
		core->hilos = (struct HILO *) malloc (sizeof(struct HILO)*num_hilos);
			for(j=0;j<num_hilos;j++){
				struct HILO * hilo = (struct HILO *) malloc(sizeof(struct HILO));
				hilo->id = j;
				hilo->core_id= i;
				hilo->estado = 0;
				core->hilos[j] = *hilo;
			}
		cpu->cores[i] = *core;
	}
	
	return cpu;	
}


/*
	Mostramos los procesos en ejecución de la CPU, 
	sacando cada proceso en ejecución en los hilos.
*/
int mostrar_procesosCPU(struct CPU * cpu){
	int i,j;
	for(i=0;i<cpu->num_cores;i++){
		printf("CORE_ID: %d \n",i);
		for(j=0;j<cpu->cores[i].num_hilos;j++){
			printf("HILO ID_: %d \n",cpu->cores[i].hilos[j].id);
			if(cpu->cores[i].hilos[j].estado==1){
				mostrar_PCB(cpu->cores[i].hilos[j].pcb);
			}	
			
		}
	}
	
	return 0;
}


/*
	Imprime por pantalla la situación de nuestra cola de hilos libros de la CPU.
*/
struct CPUQUEUE * create_CPUQUEUE(struct CPU * cpu){
	int i,j;
	struct CPUQUEUE * cola = (struct CPUQUEUE *) malloc(sizeof(struct CPUQUEUE));
	cola->size = 0;
	cola->hilos = (struct HILO **) malloc(sizeof(struct HILO)*cpu->num_cores*cpu->cores[0].num_hilos);
	for(i=cpu->num_cores-1;i>=0;i--){
		for(j=cpu->cores[i].num_hilos-1;j>=0;j--){
			struct HILO * hilo = (struct HILO *) malloc(sizeof(struct HILO));
			hilo=&cpu->cores[i].hilos[j];
			cola->hilos[cola->size] = hilo;
			cola->size++;			
		}
		
	}	
	
	return cola;
}


/*
	Imprime por pantalla la situación de nuestra cola de hilos libros de la CPU.
*/
int mostrar_colaCPU(struct CPUQUEUE  * cola){
	int i;
	for(i=0;i<cola->size;i++){
		printf("C:%d H:%d \t",cola->hilos[i]->core_id,cola->hilos[i]->id);
	}
	
	printf("\n");
	
	return 0;
}


/*
	Metemos un PCB en el ultimo hilo de nuestra cola, ocupando el hilo
	y quitandolo de la cola.
*/
int agregarProceso_PCB_CPU(struct CPU * cpu, struct CPUQUEUE * cola, struct PCB * pcb){
		if(cola->size != 0){
			cola->hilos[cola->size-1]->pcb = pcb;
			cola->hilos[cola->size-1]->estado =1;	
			cola->size--;			
			return 0;
		}else{
			return 1;
		}
}


/*
	Quitamos el PCB del hilo y volvemos a meter el hilo liberado
	a nuestra cola de hilos y nuestro PCB a nuestra cola de prioridades.
*/
int quitarProcesos_PCB_CPU(struct CPU * cpu, struct CPUQUEUE * cola, struct COLA_PRIORIDADES * cola_prioridades ){
	int i,j;
	for(i=0;i<cpu->num_cores;i++){
		for(j=0;j<cpu->cores[i].num_hilos;j++){		
			if(cpu->cores[i].hilos[j].estado==1){
				struct PCB * pcb = cpu->cores[i].hilos[j].pcb;
				cpu->cores[i].hilos[j].estado=0;						
				if(pcb->prioridad>0){				
					pcb->prioridad--;				
				}				
				encolar_proceso(cola_prioridades , pcb);

				struct HILO * hilo =&cpu->cores[i].hilos[j];
				cola->hilos[cola->size] = hilo;
				cola->size++;	
			}
		}
	}
	return 0;
		
	
}


/*
	Reducimos en 1 el tiempo restante de los procesos en ejcución
	en los hilos de la CPU y en el caso de haber terminado se retira
	el pcb del hilo, encolando el hilo en nuestra cola de hilos.
*/
int actualizar_tiempos_PCBS(struct CPU * cpu, struct CPUQUEUE * cola){
	int i,j;
		
	for(i=0;i<cpu->num_cores;i++){
		for(j=0;j<cpu->cores[i].num_hilos;j++){		
			if(cpu->cores[i].hilos[j].estado==1){
				if(cpu->cores[i].hilos[j].pcb->t_restante>0){
					cpu->cores[i].hilos[j].pcb->t_restante--;
				}else{
					struct HILO * hilo = (struct HILO *) malloc(sizeof(struct HILO));
					hilo=&cpu->cores[i].hilos[j];
					cola->hilos[cola->size] = hilo;
					cpu->cores[i].hilos[j].estado=0;	
					cola->size++;			
				}
					
			}			
		}
	}

	return 1;
}









