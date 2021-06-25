#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    
#include <time.h>   
#include "CPU.h"
#include "PCB.h"
#include "PRIORIDADES.h"
#include "estructura.h"

#define NUMCORES 4
#define NUMHILOS 4
#define NUMPROCESOS 10
#define NUMPRIORIDADES 4
#define TIEMPOESPERA 3
#define TIEMPODEVIDA 20
#define QUANTUMMAX 40
#define TIME_TO_EXIT 100
#define TIEMPO_ACTUALIZAR 5

sem_t seg;
pthread_mutex_t mutex_seg, mutex_temp, mutex_procesos;
struct COLA_PRIORIDADES * cola_prioridades;
struct CPU * cpu;
struct CPUQUEUE * colaCPU;

int temporizador;
int interr_temp;
int interr_process;

void * generador_Procesos(void *id_gen) {
    int i;
    for (i=0; i<NUMPROCESOS; i++) {
        int prio = (rand()%NUMPRIORIDADES);
        int espera = (rand()%TIEMPOESPERA);
        int quantum = (rand()%QUANTUMMAX);
        int t_vida = (rand()%TIEMPODEVIDA);
        struct PCB *pcb = crearPCB(i,t_vida,prio,quantum);
		pthread_mutex_lock(&mutex_procesos);

        encolar_proceso(cola_prioridades, pcb);
		
		interr_process=1;
        pthread_mutex_unlock(&mutex_procesos);

		sleep(espera);	
    }	

}
void *scheduler(void *s) {
    int i,j;
    while(1){
	
		pthread_mutex_lock(&mutex_procesos);				
		if(interr_process==1){			
			for(i=0;i<cpu->num_cores;i++){
				for(j=0;j<cpu->cores[i].num_hilos;j++){				
					struct PCB *pcb =desencolar_primer_proceso(cola_prioridades);
					if(pcb!=NULL){
						agregarProceso_PCB_CPU(cpu, colaCPU, pcb);						
					}
					
				}
			}
			interr_process=0;

		}
        pthread_mutex_unlock(&mutex_procesos);

		pthread_mutex_lock(&mutex_temp);				
		if(interr_temp==1){			
			for(i=0;i<cpu->num_cores;i++){

				////
				//quitarProcesos_PCB_CPU(cpu, colaCPU, cola_prioridades );
				///

				for(j=0;j<cpu->cores[i].num_hilos;j++){				
					struct PCB *pcb =desencolar_primer_proceso(cola_prioridades);
					if(pcb!=NULL){
						agregarProceso_PCB_CPU(cpu, colaCPU, pcb);						
					}
					
				}
			}
			interr_temp=0;

		}
        pthread_mutex_unlock(&mutex_temp);
		
    }
}

void *cclock(void *id_clock) {   
    while(1) {    
		sleep(1);

        pthread_mutex_lock(&mutex_seg);       

		actualizar_tiempos_PCBS(cpu,colaCPU);

		pthread_mutex_lock(&mutex_procesos);
        interr_process = 1;
        pthread_mutex_unlock(&mutex_procesos);

        pthread_mutex_unlock(&mutex_seg);
        
		
		
	
		sem_post(&seg);
		printf("PROCESOS \n\n");
		mostrar_procesosCPU(cpu);
	

		
    }
}

void *ttemp(void *id_temp) {
    while(1) {
		sem_wait(&seg);
		pthread_mutex_lock(&mutex_seg);
        
		temporizador++;
		if(temporizador==TIEMPO_ACTUALIZAR){
			temporizador=0;			 
			interr_temp=1;		
			printf("\n");
			printf("TE ACTUALIZASSSSSSS \n");	
			printf("\n");
		}
		pthread_mutex_unlock(&mutex_seg);
		

    }
}






int main(int argc, char *argv[]){
	cpu = crearCPU(1,NUMCORES,NUMHILOS);
	colaCPU =  create_CPUQUEUE(cpu);
	cola_prioridades = crear_cola_prioridades(NUMPROCESOS, NUMPRIORIDADES);
	
	struct generador_procesos * gp = (struct generador_procesos *) malloc(sizeof(struct generador_procesos));
	gp->id_gen = 0;
	struct clock * clk = (struct clock *) malloc(sizeof(struct clock));
	clk->id_clock = 0;
	struct temporizador * temp = (struct temporizador *)malloc(sizeof(struct temporizador));
	temp->id_temp = 0;
	struct scheduler * sch = (struct scheduler *)malloc(sizeof(struct scheduler));
	sch->id_sch = 0;
	
	pthread_mutex_init(&mutex_procesos, NULL);
	pthread_mutex_init(&mutex_seg, NULL);
	pthread_mutex_init(&mutex_temp, NULL);
	
	
	pthread_create(&gp->pthid,NULL,generador_Procesos,&gp->id_gen);
	pthread_create(&clk->pthid,NULL,cclock,&clk->id_clock);
	pthread_create(&temp->pthid,NULL,ttemp,&temp->id_temp);
	pthread_create(&sch->pthid,NULL,scheduler,&sch->id_sch);

	
	sem_init(&seg,0,0);


	sleep(TIME_TO_EXIT);
		
}