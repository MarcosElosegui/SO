#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "PRIORIDADES.h"
#include "PCB.h"

/*
	Creamos nuestra estructura de cola de priorides de nuestros procesos.
*/
struct COLA_PRIORIDADES * crear_cola_prioridades(int num_procesos, int prioridad){
	
	struct COLA_PRIORIDADES * cola_prioridades = (struct COLA_PRIORIDADES *) malloc(sizeof(struct COLA_PRIORIDADES));
	cola_prioridades->procesos = (struct COLA_PROCESOS *) malloc(sizeof(struct COLA_PROCESOS)*num_procesos);
	cola_prioridades->size = prioridad;
	
	int i,j;
	for(i=0;i<prioridad;i++){
		struct COLA_PROCESOS * cola_procesos = (struct COLA_PROCESOS *) malloc(sizeof(struct COLA_PROCESOS));
		cola_procesos->pcbs = (struct PCB *) malloc(sizeof(struct PCB)*num_procesos*5);
		cola_procesos->num_procesos = num_procesos;
		cola_procesos->size = 0;
		cola_procesos->prioridad = prioridad;
		cola_prioridades->procesos[i] = *cola_procesos;
		cola_procesos->pos_actual = 0;			
		cola_procesos->ultimo = 0;
		
	}
	return cola_prioridades;
} 


/*
	Encolamos la PCB pasada por parámetro en la cola de prioridades correspondiente.
*/
int encolar_proceso(struct COLA_PRIORIDADES * cola , struct PCB * pcb){

	cola->procesos[pcb->prioridad].pcbs[cola->procesos[pcb->prioridad].ultimo] = *pcb;	
	cola->procesos[pcb->prioridad].size++;
	cola->procesos[pcb->prioridad].ultimo++;

	return 0;
}

/*
	Desencolamos el primer proceso de nuestra cola de prioridades.
*/
struct PCB * desencolar_primer_proceso(struct COLA_PRIORIDADES * cola){
	int i;	

	for(i=0;i<cola->size;i++){
		if(cola->procesos[i].size >0){			
			struct PCB * pcb = &cola->procesos[i].pcbs[cola->procesos[i].pos_actual];
			cola->procesos[i].pos_actual++;
			cola->procesos[i].size--;
			return pcb;
		}
	}
	return NULL;
}

/*
	Imprimimos por pantalla el estado de nuestra cola de prioridades.
*/
int mostrar_cola_prioridades(struct COLA_PRIORIDADES * cola){
	int i,j;
	
	for(i=0; i<cola->size;i++){
		printf("Cola Procesos %d: \n", i);
		for(j=0;j<cola->procesos[i].size;j++){
			printf("Posicion %d : \t ",j);
			mostrar_PCB(&cola->procesos[i].pcbs[j]);
		}
	}
	return 0;
}
