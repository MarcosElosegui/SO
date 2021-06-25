#ifndef _PCB_h
#define _PCB_h

struct PCB {
	int id;
	int t_vida;
	int t_restante;
	int quantum;
	int estado; //1 ocupado, 0 ready
	int prioridad;		
};


struct PCB * crearPCB(int pid, int t_vida, int prioridad, int quantum);

int mostrar_PCB(struct PCB * pcb);

#endif