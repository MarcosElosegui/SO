#ifndef _memoria_h
#define _memoria_h

#define TAM_PALABRA 4
#define TAM_PAGINA 256
#define MAX_LINEAS 30
#define TAM_LINEA 20
#define TAM_DATO_CABECERA 6

struct memoria {
    int tam;
    int ini_pag;
    int fin_pag;
    int ini_kernel;
    int fin_kernel;
    int *contenido;
};

struct memoria * crearMemoria(int size_bits);
int escribirMemoria(struct memoria * memoria,int num_pag, int int_textos[MAX_LINEAS], int numli_text, int int_datos[MAX_LINEAS], int numli_data);
void mostrar_memoria(struct memoria * memoria,int num_pag);


#endif