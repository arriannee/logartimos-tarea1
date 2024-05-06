#include <pthread.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include "mtree.hpp"

// Definir el main
int main() {

    // Paso 1: Crear un conjunto de puntos de prueba
    int numPuntos = 100;  // Número de puntos
    int B = 4096 / sizeof(Entrada);  // Capacidad del nodo

    std::vector<Punto> puntos;
    srand(static_cast<unsigned int>(time(0)));  // Para números aleatorios

    for (int i = 0; i < numPuntos; ++i) {
        double x = static_cast<double>(rand()) / RAND_MAX;
        double y = static_cast<double>(rand()) / RAND_MAX;
        puntos.push_back(Punto(x, y));
    }

    // Paso 2: Construir el M-Tree usando el método SS
    MTree* mtree = SS(puntos, B);

    auto start = std::chrono::high_resolution_clock::now();

    // Paso 3: Imprimir el resultado
    // Como ejemplo, imprimamos la estructura del árbol a nivel de nodos

    std::cout << "M-Tree construido. La estructura de nodos es:\n";
    std::cout << "Nodo raíz con " << mtree->raiz->entradas.size() << " entradas.\n";

    // Limpiar memoria
    delete mtree;
    return 0;
}
