#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// Un M-Tree es un árbol que está compuesto de nodos que contiene entradas (p, cr, a).
// p es un punto
// cr es el radio covertor de este subárbol 
// a es una dirección en disco a la página de su hijo identificado por la entrada de su nodo interno

// Para contexto de esta tarea utilizaremos las coordenadas
// de p y cr como reales de doble precisión (double).

typedef struct Punto {
    double x;
    double y;
} Punto;

typedef struct NodoMTree {
    Punto p;
    double cr;
    int numHijos;
    struct NodoMTree** hijos;
} NodoMTree;

// Función para calcular la distancia euclidiana entre dos puntos 
double distancia(Punto a, Punto b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Función para crear un MTreeNode con p y cr
NodoMTree* crearNodo(Punto p, double cr) {
    NodoMTree* nodo = (NodoMTree*)malloc(sizeof(NodoMTree));
    nodo->p = p;
    nodo->cr = cr;
    nodo->numHijos = 0;
    nodo->hijos = NULL; // :,)
    return nodo;
}

// Para simplificar las implementaciones, diremos que cada nodo tendrá
// que tener una capacidad mínima de b = 0.5 · B y capacidad máxima de B.

# define b 0.5 * B 
# define B 10 // 10 por mientras pq tira error si pongo "B"

// Método Ciaccia-Patella (CP)
NodoMTree* metodoCP(int n, Punto* P) {

    // Paso 1: Si |P| ≤ B, se crea un árbol T , se insertan todos los puntos a T y se retorna T
    if (n <= B) {
        NodoMTree* nodo = crearNodo(P[0], 0.0); // Crea el primer nodo con el primer punto
        return nodo;
    }

    // Paso 2: De manera aleatoria se eligen k = min(B, n/B) puntos de P, que los llamaremos samples pf1, ..., pfk. Se insertan en un conjunto F de samples
    int k = (n < B) ? n : B;  // Número de samples
    Punto* samples = (Punto*)malloc(k * sizeof(Punto)); // Conjunto F de samples

    for (int i = 0; i < k; i++) {
        samples[i] = P[rand() % n]; // Elegir samples aleatorios
    }


    // Paso 3: Asignar puntos a su sample más cercano para formar clusters
    int* clusterSize = (int*)malloc(k * sizeof(int)); // Tamaño de cada cluster
    for (int i = 0; i < k; i++) {
        clusterSize[i] = 0;
    }

    Punto* clusters[k]; // Conjuntos F1, ..., Fk (Cómo arreglo esto?)
    for (int i = 0; i < k; i++) {
        clusters[i] = (Punto*)malloc(n * sizeof(Punto));
    }

    for (int i = 0; i < n; i++) {
        Punto p = P[i];
        int sampleMasCercano = 0;
        double minDistancia = distancia(p, samples[0]);

        for (int j = 1; j < k; j++) {
            double distance = distancia(p, samples[j]);
            if (distance < minDistancia) {
                sampleMasCercano = j;
                minDistancia = distance;
            }
        }

        clusters[sampleMasCercano][clusterSize[sampleMasCercano]++] = p; // Añadir al cluster
    }

    // Paso 4: . Etapa de redistribución: Si algún Fj es tal que |Fj| < b:
    // 4.1 Quitamos pfj de F 
    for (int i = 0; i < k; i++) {
        if (clusterSize[i] < b) {
            // remover samples[i] (pfj)
            // definir p 
            Punto p = 
            // redefinir minDistancia
            double minDistancia =
            double sampleMasCercano = 
            // redefinir sampleMasCercano

            // 4.2 Por cada p ∈ Fj, le buscamos el sample pfl más cercano de F y lo añadimos a su conjunto Fl.
            for (int l = 0; l < k; l++) {
                    // Ignorar el mismo cluster
                    double distance = distancia(p, samples[l]);
                    if (distance < minDistancia) {
                        sampleMasCercano = l;
                        minDistancia = distance;
                    }
            }

            clusters[sampleMasCercano][clusterSize[sampleMasCercano]++] = p;
        }
    }

    // Paso 5: Si |F| = 1, volver al paso 2.

    if (sizeof(samples) / sizeof(samples[0]) = 1) {
        //plop
    }

}

// Método Sexton-Swinbank (SS)


