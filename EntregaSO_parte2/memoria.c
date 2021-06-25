#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    
#include <time.h>  
#include "memoria.h"

#define INI_KERNEL 0x000000
#define FIN_KERNEL 0x3FFFFF
#define INI_PAG 0x000000


/*
    Iniciamos nuestra estructura de memoria recibiendo por el tamaño
    de esta por parámetro.
*/
struct memoria * crearMemoria(int size_bits) {
    
    struct memoria * memoria = (struct memoria *)malloc(sizeof(struct memoria));
    memoria->ini_kernel = INI_KERNEL;
    memoria->fin_kernel = FIN_KERNEL;  
    memoria->ini_pag = INI_PAG;
    int tam = 1 << size_bits;
    memoria->contenido = (int *)malloc(sizeof(int)*tam);
    memoria->tam = tam;  
    memoria->fin_pag = tam/256-1;
   

    return memoria;
}


/*
    Comprobamos si hay espacio en memoria, para poder escribir en esta.
*/
int comprobar_espacio(struct memoria * memoria,int num_pag){
    int desplazamiento=0;
    int hay_sitio=0;
    int pag_actual=0;
    int cont_pagina =num_pag;  
    while(hay_sitio ==0 && desplazamiento<memoria->fin_pag){       
        if(memoria->contenido[INI_PAG+desplazamiento]==1){
            desplazamiento++;
            pag_actual=desplazamiento;
        }else{
            cont_pagina--;
            if(cont_pagina==0){
                hay_sitio=1;
            }else{
                desplazamiento++;
            }
        }
    }    
    //printf("VALOR DE PAG ACTUAL = %d",pag_actual);
    if(hay_sitio==0)return -1;    
    return pag_actual;
}


/*
    Escribimos en memoria los datos pasados por parámetro.
*/
int escribirMemoria(struct memoria * memoria,int num_pag, int int_textos[MAX_LINEAS], int numli_text, int int_datos[MAX_LINEAS], int numli_data){
    
   int pag_actual = comprobar_espacio(memoria,num_pag);  
    
    if(pag_actual!=-1){
        int h;
        //escribimos las instrucciones y datos en memoria        
        //1-Ocupamos la memoria
        for(h=0;h<num_pag;h++){
             memoria->contenido[pag_actual+h] = 1;
        }        
        //2-Agregamos las instrucciones
        for(h=0;h<numli_text;h++){
            memoria->contenido[FIN_KERNEL+1+TAM_PAGINA*pag_actual+h*TAM_PALABRA]=int_textos[h];
        }
        //3-Agregamos a la memoria los datos
        for(h=0;h<numli_data;h++){
            memoria->contenido[FIN_KERNEL+1+TAM_PAGINA*pag_actual+h*TAM_PALABRA+numli_text*TAM_PALABRA] = int_datos[h];
        }
    }else{
        printf("No hay espacio para almacenar el contenido del programa \n");
    }
    return pag_actual;
}

/*
    Se imprime toda la memoria por pantalla.
*/
void mostrar_memoria(struct memoria * memoria,int num_pag){
    printf("DATOS DE LA MEMORIA \n");
    int dir_virt;
    int cont = 0;
    printf("NumPag: %d\n",num_pag);
    for(dir_virt=INI_PAG;dir_virt<num_pag*TAM_PAGINA;dir_virt+=4){
        int direccion_mem = memoria->fin_kernel+1+dir_virt;
        printf("En la posicion De Memoria:: %06X   VALOR-> %08X \n",direccion_mem,memoria->contenido[direccion_mem]);
            //printf("NumPag: %d\n",num_pag);
            cont++;
    }
    printf("Fin: cont %d \n",cont);
}