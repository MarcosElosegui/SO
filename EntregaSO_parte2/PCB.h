#ifndef _PCB_h
#define _PCB_h

struct PCB {
	int id;
	int t_vida;
	int t_restante;
	int quantum;
	int estado; //1 ocupado, 0 ready
	int prioridad;		
	struct MM * mm;	
	int PC;
    int PTBR;
};

struct MM{
	int codigo;
	int datos;
	int pgb;
};


struct PCB * crearPCB(int pid, int t_vida, int prioridad, int quantum,struct MM * mm);
struct MM * crearMM(int codigo, int datos, int pgb);

int mostrar_PCB(struct PCB * pcb);

#endif