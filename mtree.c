#include "mtree.h"

// Función para crear un nuevo nodo.
Nodo *crearNodo(int esHoja, int B) {
    Nodo *nuevoNodo = (Nodo *)malloc(sizeof(Nodo));
    nuevoNodo->esHoja = esHoja;
    nuevoNodo->numEntradas = 0;
    nuevoNodo->entradas = (Entrada *)malloc(B * sizeof(Entrada));
    return nuevoNodo;
}

// Función para inicializar el M-Tree.
MTree *crearMTree(int B) {
    MTree *mtree = (MTree *)malloc(sizeof(MTree));
    mtree->B = B;
    mtree->raiz = crearNodo(1, B);  // Inicialmente, la raíz es una hoja.
    return mtree;
}

// Función para calcular la distancia euclidiana entre dos puntos.
double distanciaEuclidiana(Punto p1, Punto p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}
