#ifndef MTREE_H
#define MTREE_H

#include <stdlib.h>
#include <math.h>

// Definición de la estructura para las coordenadas de un punto.
typedef struct Punto {
    double x;
    double y;
} Punto;

// Definición de la estructura de una entrada en el M-Tree.
typedef struct Entrada {
    Punto p;       // El punto en el espacio.
    double cr;     // Radio cobertor.
    struct Nodo *hijo; // Puntero al nodo hijo (solo para nodos internos).
} Entrada;

// Definición de la estructura del nodo del M-Tree.
typedef struct Nodo {
    int esHoja;          // Para indicar si el nodo es una hoja.
    int numEntradas;     // Número de entradas almacenadas en el nodo.
    Entrada *entradas;   // Array de entradas.
    struct Nodo *hijo;
} Nodo;

typedef struct MTree {
    Nodo *raiz;
    int B;   // Capacidad máxima de entradas en cada nodo.
} MTree;

typedef struct Cluster {
    Punto *puntos;    // Array dinámico de puntos en el cluster.
    int numPuntos;    // Número actual de puntos en el cluster.
    int capacidad;    // Capacidad máxima del array de puntos.
    Punto medoide;    // Medoide actual del cluster.
    double radioCobertor; 
} Cluster;

// Función para crear un nuevo nodo.
Nodo *crearNodo(int esHoja, int B);

// Función para inicializar el M-Tree.
MTree *crearMTree(int B);

// Función para calcular la distancia euclidiana entre dos puntos.
double distanciaEuclidiana(Punto p1, Punto p2);

#endif /* MTREE_H */
