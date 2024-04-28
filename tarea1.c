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

# define B 10  // 10 por mientras pq tira error si pongo "B"
# define b 0.5 * B 


/*******************************
Métodos para construir un M-Tree
********************************/

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

// Para explicar el algoritmo bulk-loader se definen las siguientes funciones:

// Cluster: retorna un conjunto de clusters de tamaño entre b y B.

// Estructura para un cluster
typedef struct {
    Punto* points;  // Puntos en el cluster
    int count;      // Número de puntos
} Cluster;

// Función para encontrar los pares más cercanos de clusters
void find_closest_clusters(Cluster* clusters, int numClusters, int* idx1, int* idx2) {
    double min_dist = 0; 
    for (int i = 0; i < numClusters; i++) {
        for (int j = i + 1; j < numClusters; j++) {
            double dist = distancia(clusters[i].points[0], clusters[j].points[0]);
            if (dist < min_dist) {
                min_dist = dist;
                *idx1 = i;
                *idx2 = j;
            }
        }
    }
}

// Función de cluster para agrupar puntos en clusters de tamaño entre b y B
// Input: Un set de puntos Cin de tamaño mínimo b
Cluster* cluster(Punto* Cin, int point_count) {

    // Paso 1: Se define Cout = {} y C = {}
    Cluster* Cout = (Cluster*)malloc(point_count * sizeof(Cluster));
    Cluster* C = (Cluster*)malloc(point_count * sizeof(Cluster));
    int count_C = 0; // Número de clusters en C
    int count_Cout = 0; // Número de clusters en Cout

    // Paso 2: Por cada punto p ∈ Cin se añade {p} a C
    for (int i = 0; i < point_count; i++) {
        C[count_C].points = (Punto*)malloc(sizeof(Punto));
        C[count_C].points[0] = Cin[i];
        C[count_C].count = 1;
        count_C++;
    }

    // Paso 3: Mientras |C| > 1 (mientras haya más de un cluster en C)
    while (count_C > 1) {
        int idx1 = -1;
        int idx2 = -1;

        // Paso 3.1: Encontrar los pares más cercanos de clusters
        find_closest_clusters(C, count_C, &idx1, &idx2);

        // Tal que |c1| >= |c2|
        // ...

        // Paso 3.2: Si |c1 ∪ c2| ≤ B, se remueve c1 y c2 de C y se añade c1 ∪ c2 a C.
        // (Si la unión de los clusters más cercanos no excede B)
        if (idx1 >= 0 && idx2 >= 0) {
            if ((C[idx1].count + C[idx2].count) <= B) {
                // Combinar los clusters
                Punto* new_points = (Punto*)malloc((C[idx1].count + C[idx2].count) * sizeof(Punto));
                int index = 0;

                // Copiar puntos del primer cluster
                for (int i = 0; i < C[idx1].count; i++) {
                    new_points[index++] = C[idx1].points[i];
                }

                // Copiar puntos del segundo cluster
                for (int i = 0; i < C[idx2].count; i++) {
                    new_points[index++] = C[idx2].points[i];
                }

                free(C[idx1].points);
                free(C[idx2].points);

                // No sé cómo seguir aquí
            } else {
                // Paso 3.2: Si no, se remueve c1 de C y se añade c1 a Cout.
                Cout[count_Cout++] = C[idx1];

                for (int i = idx1; i < count_C - 1; i++) {
                    C[i] = C[i + 1];
                }

            count_C--; // Reducir el número total de clusters
            }
        }
    }

    // Paso 4: Sea c el último elemento de C
    Cluster c = C[0];

    // Paso 5: Si hay elementos en Cout
    if (count_Cout > 0) {
        // Paso 5.1 definimos c′ como el vecino más cercano a c en Cout. Removemos c′ de Cout.
        Cluster c_prima; // Incompleto
    }

}

NodoMTree* metodoSS(Punto* points, int n) {

}
