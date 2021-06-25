#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "PCB.h"

/*
	Crea un PCB con los valores dados.
*/
struct PCB * crearPCB(int pid, int t_vida, int prioridad, int quantum,struct MM * mm){
	struct PCB * pcb = (struct PCB *) malloc (sizeof(struct PCB));
	pcb->id = pid;
	pcb->t_vida = t_vida;
	pcb->t_restante = pcb->t_vida;
	pcb->PTBR=mm->pgb;
	pcb->mm=mm;
	pcb->quantum = quantum;
	pcb->estado = 0; //1 ocupado, 0 ready
	pcb->prioridad = prioridad;	
	pcb->PC=0;
	
	return pcb;
}


/*
	Crea un la estructura de un PCB.
*/
struct MM * crearMM(int codigo, int datos, int pgb){
	struct MM * mm = (struct MM *) malloc(sizeof(struct MM));
	mm->codigo=codigo;
	mm->datos = datos;
	mm->pgb = pgb;	
	
	return mm;
}

/*
	Muestra los datos de la PCB pasada por parámetro por pantalla.
*/
int mostrar_PCB(struct PCB * pcb){
	printf("PCB -> id = %d, t_vida = %d, t_restante = %d, quantum = %d, estado = %d, prioridad = %d \n",
	pcb->id,pcb->t_vida,pcb->t_restante,pcb->quantum,pcb->estado,pcb->prioridad);
printf("\t MM -> Codigo: %d, Datos: %d, PGB: %d \n",pcb->mm->codigo,pcb->mm->datos,pcb->mm->pgb);
	
	return 1;
}