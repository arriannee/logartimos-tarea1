#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

void calcularMedoide(Cluster *cluster) {
    double minDistancia = INFINITY, sumaDistancias;
    int medoideIndex = 0;

    for (int i = 0; i < cluster->numPuntos; i++) {
        sumaDistancias = 0;
        for (int j = 0; j < cluster->numPuntos; j++) {
            sumaDistancias += distanciaEuclidiana(cluster->puntos[i], cluster->puntos[j]);
        }
        if (sumaDistancias < minDistancia) {
            minDistancia = sumaDistancias;
            medoideIndex = i;
        }
    }
    cluster->medoide = cluster->puntos[medoideIndex];
}

void calcularRadioCobertor(Cluster *cluster) {
    double maxDist = 0;
    for (int i = 0; i < cluster->numPuntos; i++) {
        double dist = distanciaEuclidiana(cluster->medoide, cluster->puntos[i]);
        if (dist > maxDist) {
            maxDist = dist;
        }
    }
    cluster->radioCobertor = maxDist;
}

double distanciaEntreClusters(Cluster c1, Cluster c2) {
    return distanciaEuclidiana(c1.medoide, c2.medoide);
}

int encontrarVecinoMasCercano(Cluster *clusters, int numClusters, Cluster *targetCluster) {
    double minDistancia = INFINITY;
    int indiceVecino = -1;
    for (int i = 0; i < numClusters; i++) {
        double dist = distanciaEntreClusters(*targetCluster, clusters[i]);
        if (dist < minDistancia && targetCluster != &clusters[i]) {  // Asegúrate de no compararlo consigo mismo
            minDistancia = dist;
            indiceVecino = i;
        }
    }
    return indiceVecino;
}

// Función para encontrar los pares más cercanos
int encontrarParesMasCercanos(Cluster *clusters, int numClusters, int *indice1, int *indice2) {
    double minDistancia = INFINITY;
    int found = 0;

    for (int i = 0; i < numClusters; i++) {
        for (int j = i + 1; j < numClusters; j++) {
            double dist = distanciaEntreClusters(clusters[i], clusters[j]);
            if (dist < minDistancia) {
                minDistancia = dist;
                *indice1 = i;
                *indice2 = j;
                found = 1;
            }
        }
    }

    return found ? 0 : -1;  // Retornar 0 si se encontró un par, -1 si no
}

Cluster *crearCluster(int capacidadInicial) {
    Cluster *cluster = malloc(sizeof(Cluster));
    if (cluster == NULL) {
        perror("No se pudo asignar memoria para el cluster");
        exit(EXIT_FAILURE);
    }
    cluster->puntos = malloc(capacidadInicial * sizeof(Punto));
    if (cluster->puntos == NULL) {
        perror("No se pudo asignar memoria para los puntos del cluster");
        free(cluster);
        exit(EXIT_FAILURE);
    }
    cluster->numPuntos = 0;
    cluster->capacidad = capacidadInicial;
    return cluster;
}

void agregarPuntoACluster(Cluster *cluster, Punto punto) {
    if (cluster->numPuntos >= cluster->capacidad) {
        cluster->capacidad *= 2; // Dobla la capacidad si es necesario
        cluster->puntos = realloc(cluster->puntos, cluster->capacidad * sizeof(Punto));
        if (cluster->puntos == NULL) {
            perror("No se pudo reasignar memoria para los puntos del cluster");
            exit(EXIT_FAILURE);
        }
    }
    cluster->puntos[cluster->numPuntos++] = punto; // Agrega el punto y incrementa el contador
}

// Función para combinar dos clusters
Cluster combinarClusters(Cluster *c1, Cluster *c2) {
    // Suponiendo que ambos clusters ya tienen memoria asignada para sus puntos
    Cluster nuevo;
    nuevo.puntos = malloc((c1->numPuntos + c2->numPuntos) * sizeof(Punto));
    memcpy(nuevo.puntos, c1->puntos, c1->numPuntos * sizeof(Punto));
    memcpy(nuevo.puntos + c1->numPuntos, c2->puntos, c2->numPuntos * sizeof(Punto));
    nuevo.numPuntos = c1->numPuntos + c2->numPuntos;

    // Calcular nuevo medoide, etc.
    calcularMedoide(&nuevo);

    return nuevo;
}

void splitMinMaxPolicy(Cluster *original, Cluster *split1, Cluster *split2, int B) {
    double minMaxRadio = INFINITY;
    // Variables temporales para los splits de prueba
    Cluster tempSplit1, tempSplit2;

    // Iterar sobre todos los pares posibles de puntos para considerar como centros iniciales
    for (int i = 0; i < original->numPuntos; i++) {
        for (int j = i + 1; j < original->numPuntos; j++) {
            // Inicializar los splits temporales
            tempSplit1.puntos = malloc(original->numPuntos * sizeof(Punto));
            tempSplit2.puntos = malloc(original->numPuntos * sizeof(Punto));
            tempSplit1.numPuntos = 0;
            tempSplit2.numPuntos = 0;

            // Asignar cada punto al grupo más cercano
            for (int k = 0; k < original->numPuntos; k++) {
                if (k != i && k != j) {
                    if (distanciaEuclidiana(original->puntos[k], original->puntos[i]) < distanciaEuclidiana(original->puntos[k], original->puntos[j])) {
                        tempSplit1.puntos[tempSplit1.numPuntos++] = original->puntos[k];
                    } else {
                        tempSplit2.puntos[tempSplit2.numPuntos++] = original->puntos[k];
                    }
                }
            }
            
            // Agregar los centros iniciales a los clusters
            tempSplit1.puntos[tempSplit1.numPuntos++] = original->puntos[i];
            tempSplit2.puntos[tempSplit2.numPuntos++] = original->puntos[j];

            // Calcular el medoide y el radio cobertor para cada split
            calcularMedoide(&tempSplit1);
            calcularRadioCobertor(&tempSplit1);
            calcularMedoide(&tempSplit2);
            calcularRadioCobertor(&tempSplit2);
            double maxRadio = fmax(tempSplit1.radioCobertor, tempSplit2.radioCobertor);

            // Verificar si esta configuración es la mejor
            if (maxRadio < minMaxRadio) {
                minMaxRadio = maxRadio;
                // Copiar los splits a las estructuras de salida si es necesario
                if (split1 && split2) {
                    free(split1->puntos);
                    free(split2->puntos);
                    *split1 = tempSplit1;
                    *split2 = tempSplit2;
                }
            } else {
                // Liberar memoria si no es la mejor configuración
                free(tempSplit1.puntos);
                free(tempSplit2.puntos);
            }
        }
    }
}


// Implementa el algoritmo de clustering según los pasos detallados.
Cluster *cluster(Punto *Cin, int numPuntos, int b, int B) {
    // Paso 1: Inicialización de los arreglos de clusters C y Cout
    Cluster *C = malloc(numPuntos * sizeof(Cluster)); // Almacena los clusters temporales
    Cluster *Cout = malloc(numPuntos * sizeof(Cluster)); // Almacena los clusters finales
    int numC = 0, numCout = 0; // Contadores para C y Cout

    // Paso 2: Crear un cluster inicial para cada punto
    for (int i = 0; i < numPuntos; i++) {
        C[numC] = *crearCluster(1); // Suponer una función que crea un cluster con capacidad inicial
        agregarPuntoACluster(&C[numC++], Cin[i]); // Suponer una función que añade un punto a un cluster
    }

    // Paso 3: Proceso de combinación de clusters en C
    while (numC > 1) {
        int idx1, idx2;

        // Paso 3.1: Encontrar los dos clusters más cercanos
        if (encontrarParesMasCercanos(C, numC, &idx1, &idx2) == -1) {
            break;  // Salir si no hay pares para combinar
        }

        // Paso 3.2: Combinar clusters si es posible
        if (C[idx1].numPuntos + C[idx2].numPuntos <= B) {
            C[idx1] = combinarClusters(&C[idx1], &C[idx2]); // Combinar los dos clusters
            C[idx2] = C[--numC]; // Eliminar el segundo cluster moviéndolo al final y reduciendo el contador
        } else {
            // Mover el cluster que no se puede combinar a Cout
            Cout[numCout++] = C[idx2];
            C[idx2] = C[--numC]; // Reducir numC ya que un cluster ha sido movido a Cout
        }
    }

    // Paso 4: Manejar el último cluster en C
    if (numC == 1) {
        // Paso 5 y 6: Tratar de combinar con un cluster en Cout si es posible
        if (numCout > 0) {
            int idx = encontrarVecinoMasCercano(Cout, numCout, &C[0]);
            if (C[0].numPuntos + Cout[idx].numPuntos <= B) {
                Cout[idx] = combinarClusters(&C[0], &Cout[idx]); // Combinar y actualizar en Cout
            } else {
                // Dividir usando MinMax si la combinación no es posible
                splitMinMaxPolicy(&C[0], &Cout[idx], b, B); // Suponer una función que aplica la política MinMax
            }
        } else {
            Cout[numCout++] = C[0]; // Simplemente mover a Cout si no hay vecinos
        }
    }

    // Paso 7: Retornar el array de clusters procesados en Cout
    free(C); // Liberar memoria de C
    return Cout;
}

Nodo *OutputHoja(Cluster *Cin, int numPuntos) {}

Nodo *OutputInterno() {}





int main() {
    return 0;
}