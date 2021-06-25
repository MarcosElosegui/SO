#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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
				hilo->registros = (int *) malloc(sizeof(int)*16);
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
			printf(" \t HILO ID_: %d \n",cpu->cores[i].hilos[j].id);
			if(cpu->cores[i].hilos[j].estado==1){
				mostrar_PCB(cpu->cores[i].hilos[j].pcb);
			}	
			
		}
	}
	
	return 0;
}

/*
	Crea una cola ordenada con todos los hilos libres de nuestra CPU.
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
int ejecutar_instruccion(struct CPU * cpu, struct CPUQUEUE * cola, struct memoria * memoria){
	int i,j;
		
	for(i=0;i<cpu->num_cores;i++){
		for(j=0;j<cpu->cores[i].num_hilos;j++){		
			if(cpu->cores[i].hilos[j].estado==1){
				if(cpu->cores[i].hilos[j].pcb->t_restante>0){
					//mantenemos
					leer_instruccion(memoria,cpu->cores[i].hilos[j].pcb,&cpu->cores[i].hilos[j]);
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

/*
	Lee y ejecuta las instrucciones del proceso leidas desde nuestra memoria.
*/
void leer_instruccion(struct memoria * memoria, struct PCB * pcb,struct HILO * hilo){
	int i;
	//printf("CONTENIDO QUE VA A LEER EL HILO %d DE LA DIRECCION %06X   ...  ES %08X \n",hilo->id,memoria->fin_kernel+pcb->PC+pcb->PTBR*TAM_PAGINA+1,memoria->contenido[1+memoria->fin_kernel+pcb->PC+pcb->PTBR*TAM_PAGINA]);
	char * contenido = (char *)malloc(sizeof(char));
	sprintf(contenido, "%X", memoria->contenido[1+memoria->fin_kernel+pcb->PC+pcb->PTBR*TAM_PAGINA]);
	//printf(" longitud %ld \n",memoria->contenido[1+memoria->fin_kernel+pcb->PC+pcb->PTBR*TAM_PAGINA]);
	//printf("CONTENIDO QUE VA A LEER EL HILO %d ES %s \n",hilo->id,contenido);	
	//sprintf(contenido, "%d", memoria->contenido[1+memoria->fin_kernel+pcb->PC+pcb->PTBR*TAM_PAGINA]);
	//printf("LONGITUD DEL DATO %ld con el contenido %s \n",strlen(contenido),contenido);
	char * resultado=(char *)malloc(sizeof(char));
	for(i=0; i<8-strlen(contenido);i++){
		strcat(resultado, "0");	
	}
	strcat(resultado,contenido);

	int r1=0;
	int r2=0;
	int r3=0;
	char direc_dato[6];
	int direc_mem;
	int aux;
	int posicion_f; 
	int posicion_a;	
	int suma=0;
	switch(resultado[0]){
		case '0':		
			r1 = resultado[1]-'0';		
			//printf("R1 -> %d \n", r1);		
			strncpy(direc_dato,&resultado[2],6);	
		
			aux =  (int)strtol(direc_dato,NULL,16);
			//printf("AUX  = %d \n",aux);
			hilo->registros[r1]=memoria->contenido[1+memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux];		
			//posicion_f = 1+memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux;
			//printf("DIRECCION REAL %08X \n",posicion_f);
		//	printf("EL VALOR QUE HA TOMADO R1 DE LA DIRECCION REAL ES %d \n", hilo->registros[r1]);
			printf("SE ESTA EJECUTANDO [ ld , r%d , %0X0]   \n",r1,1+memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux);
			break;

		case '1':
		//	printf("Esto es un Storage \n");
			r1 = resultado[1]-'0';		
			//printf("R1 -> %d \n", r1);		
			strncpy(direc_dato,&resultado[2],6);				
			aux =  (int)strtol(direc_dato,NULL,16);
			//printf("AUX  = %d \n",aux);		
			memoria->contenido[memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux+pcb->mm->codigo]=hilo->registros[r1];
		//	printf("EL VALOR QUE HA TOMADO LA DIRECCION %08X  ES %d \n",memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux+pcb->mm->codigo, hilo->registros[r1]);
			printf("SE ESTA EJECUTANDO [ st , r%d , %0X0]   \n",r1,memoria->fin_kernel+pcb->PTBR*TAM_PAGINA+aux+pcb->mm->codigo);
			
			break;
		case '2':
			//printf("Esto es un add \n");
			r1 = resultado[1]-'0';	
			r2 = resultado[2]-'0';
			r3 = resultado[3]-'0';	
			//printf("R1 -> %d \n", r1);	

			suma = hilo->registros[r2]+hilo->registros[r3];
			//printf("SE VA A SUMAR %d + %d \n",hilo->registros[r2],hilo->registros[r3]);
			hilo->registros[r1] = suma; 
			//printf("RESULTADO : %d", hilo->registros[r1]);
			printf("SE ESTA EJECUTANDO [ add , r%d , r%d, r%d ]   \n",r1,r2,r3);
			break;
		case 'F':
			printf("Esto es un EXIT \n");

			break;
		
	}
	

	pcb->PC+=4;

}








