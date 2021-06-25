#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    
#include <time.h>   
#include <string.h>

#include "CPU.h"
#include "PCB.h"
#include "PRIORIDADES.h"
#include "estructura.h"
#include "memoria.h"

#define NUMCORES 4
#define NUMHILOS 4
#define NUMPROCESOS 10
#define NUMPRIORIDADES 4
#define TIEMPOESPERA 3
#define TIEMPODEVIDA 20
#define QUANTUMMAX 40
#define TIME_TO_EXIT 100
#define TIEMPO_ACTUALIZAR 5

#define TAM_MEM 24



sem_t seg;
pthread_mutex_t mutex_seg, mutex_temp, mutex_procesos;
struct COLA_PRIORIDADES * cola_prioridades;
struct CPU * cpu;
struct CPUQUEUE * colaCPU;
struct memoria * memoria;

int temporizador;
int interr_temp;
int interr_process;
FILE * f;
int total_pag=0;

/*
	Hilo loader. Se ocupa de leer los programas .elf, guardarlos en memoria
	y cargarlos en nuestra cola de prioridades de procesos.
*/
void * lloader(void *id_load) {
   	
	char nombre_prog[11] = "prog000.elf";	
	
	int pcb_pid=0;
    while((f=fopen(nombre_prog,"r"))!=NULL) {
 		int i=0;
		printf("Lectura de programa %s \n",nombre_prog);
		int numli_text=0;
		int numli_datos=0;
		int PC =0;
		

		char valor_linea_char[TAM_LINEA];
		char valor_contenido_cabecera[TAM_DATO_CABECERA];
		int valor_linea_int;

		int int_textos[MAX_LINEAS];
		int int_datos[MAX_LINEAS];

		fgets(valor_linea_char,TAM_LINEA,f);
		strncpy(valor_contenido_cabecera,valor_linea_char+6,6);
		valor_linea_int = (int)strtol(valor_contenido_cabecera,NULL,16);
		//printf("La direccion de INICIO de las INSTRUCCIONES es %06X \n",valor_linea_int);

		fgets(valor_linea_char,TAM_LINEA,f);		
		strncpy(valor_contenido_cabecera,valor_linea_char+6,6);
		valor_linea_int = (int)strtol(valor_contenido_cabecera,NULL,16);
		//printf("La direccion de INICIO de los REGISTROS es %06X \n",valor_linea_int);

		while(valor_linea_int>PC){
			fgets(valor_linea_char,TAM_LINEA,f);			
			int_textos[i]=(int)strtol(valor_linea_char,NULL,16);
			//printf("\t Guardada la linea de TEXTO : %08X \n",int_textos[i]);
			PC+=TAM_PALABRA;
			i++;
			numli_text++;
		}
		i=0;
		while(fgets(valor_linea_char,TAM_LINEA,f)!=NULL){
			int_datos[i]=(int) strtol(valor_linea_char,NULL,16);
			//printf("\t Guardada la linea de DATOS : %08X \n",int_datos[i]);
			i++;
			numli_datos++;
		}

	
		int tam_total = numli_datos+numli_text;
		//printf("NUMERO TOTAL: %d \n",tam_total);
		int num_pag;
		//YA NOS LIAREMOS
        if (tam_total%64 != 0) {
            num_pag = tam_total/64 + 1;
        } else {
            num_pag = tam_total/64;
        }
		total_pag+=num_pag;
		fclose(f);
		int PTBR = escribirMemoria(memoria,num_pag, int_textos, numli_text,int_datos, numli_datos);
		struct MM * mm = crearMM(numli_text,numli_datos,PTBR);
		int prio = (rand()%NUMPRIORIDADES);      
        int quantum = (rand()%QUANTUMMAX);    
		struct PCB * pcb = crearPCB(pcb_pid,numli_text,prio,quantum,mm);
	
		int j;
		char * num_prog = (char *)malloc(sizeof(char));		
		for(j=0;j<3;j++)num_prog[j]=nombre_prog[4+j];
		sprintf(num_prog, "%d", atoi(num_prog)+1);

		
		strcpy(nombre_prog,"prog");		
		if(strlen(num_prog)<2)strcat(nombre_prog,"00");else(strcat(nombre_prog,"0"));
		strcat(nombre_prog,num_prog);		
		strcat(nombre_prog,".elf");
		//printf("Siguiente programa NOMBRE:= %s \n", nombre_prog);

      
		pthread_mutex_lock(&mutex_procesos);
        encolar_proceso(cola_prioridades, pcb);		
		interr_process=1;
        pthread_mutex_unlock(&mutex_procesos);
   		int espera = ((rand()%TIEMPOESPERA) +1);
		pcb_pid++;
		sleep(espera);		
    }	
	printf("acaba \n");

}

/*
	Hilo scheduler. Se ocupa de cargar el primer proceso ordenado por prioridades y
	meterlo en la CPU para ejecutarlo en 1 hilo de esta.
*/
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
		
    }
}

/*
	Hilo clock. Se ocupa de ejecutar las instrucciones leidas de memoria.
*/
void *cclock(void *id_clock) {   
    while(1) {    
		sleep(1);

        pthread_mutex_lock(&mutex_seg);       

		ejecutar_instruccion(cpu,colaCPU,memoria);

		pthread_mutex_lock(&mutex_procesos);
        interr_process = 1;
        pthread_mutex_unlock(&mutex_procesos);

        pthread_mutex_unlock(&mutex_seg);
        
		
		
	
		sem_post(&seg);
		printf("PROCESOS \n\n");
		mostrar_procesosCPU(cpu);
		//mostrar_memoria(memoria,total_pag);
	

		
    }
}

/*
*/
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
	memoria = crearMemoria(24);

	struct loader * gp = (struct loader *) malloc(sizeof(struct loader));
	gp->id_load = 0;
	struct clock * clk = (struct clock *) malloc(sizeof(struct clock));
	clk->id_clock = 0;
	struct temporizador * temp = (struct temporizador *)malloc(sizeof(struct temporizador));
	temp->id_temp = 0;
	struct scheduler * sch = (struct scheduler *)malloc(sizeof(struct scheduler));
	sch->id_sch = 0;
	
	pthread_mutex_init(&mutex_procesos, NULL);
	pthread_mutex_init(&mutex_seg, NULL);
	pthread_mutex_init(&mutex_temp, NULL);
	
	
	pthread_create(&gp->pthid,NULL,lloader,&gp->id_load);
	pthread_create(&clk->pthid,NULL,cclock,&clk->id_clock);
	pthread_create(&temp->pthid,NULL,ttemp,&temp->id_temp);
	pthread_create(&sch->pthid,NULL,scheduler,&sch->id_sch);

	
	sem_init(&seg,0,0);


	sleep(TIME_TO_EXIT);
		
}