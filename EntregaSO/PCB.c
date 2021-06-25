#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "PCB.h"


struct PCB * crearPCB(int pid, int t_vida, int prioridad, int quantum){
	struct PCB * pcb = (struct PCB *) malloc (sizeof(pcb));
	pcb->id = pid;
	pcb->t_vida = t_vida;
	pcb->t_restante = pcb->t_vida;
	pcb->quantum = quantum;
	pcb->estado = 0; //1 ocupado, 0 ready
	pcb->prioridad = prioridad;	
	
	return pcb;
}

int mostrar_PCB(struct PCB * pcb){
	printf("PCB -> id = %d, t_vida = %d, t_restante = %d, quantum = %d, estado = %d, prioridad = %d \n",
	pcb->id,pcb->t_vida,pcb->t_restante,pcb->quantum,pcb->estado,pcb->prioridad);
	return 1;
}