#include <pthread.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

#include "mtree.hpp"

// Función para contar el número total de objetos en el M-Tree
int contarNodo(Nodo* nodo) {
    if (!nodo) {
        return 0;
    }

    int total = nodo->entradas.size();  // Contar las entradas en el nodo actual
    for (const auto& entrada : nodo->entradas) {
        total += contarNodo(nodo->entradas.a);  // Sumar los objetos en el subárbol
    }

    return total;
}

// Función para calcular la altura mínima del M-Tree
int alturaMinima(const MTree& arbol) {
    int n = contarNodo(arbol.raiz);
    int B = arbol.raiz->B;
    return std::ceil(std::log(n + 1) / std::log(B + 1));
}