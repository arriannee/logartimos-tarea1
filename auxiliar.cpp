#include <limits>
#include <algorithm>
#include <vector>
#include <tuple>
#include "mtree.hpp"

void borrarPuntoDeVector(Punto p, vector<Punto> V) {
	V.erase(remove_if(V.begin(), V.end(), [&p](const Punto& punto){ 
                    return punto.x == p.x && punto.y == p.y; 
                }), V.end());
}

// Función recursiva para buscar un nodo con puntero nulo
tuple<Nodo*, int> buscarNodoNulo(Nodo* nodo, int alturaActual) {
    // Recorre todas las entradas del nodo
    for (Entrada entrada : nodo->entradas) {
        // Si el puntero es nulo, devuelve este nodo y la altura actual
        if (entrada.a == nullptr) {
            return make_tuple(nodo, alturaActual);
        }
        // Si el puntero no es nulo, busca en el siguiente nodo
        else {
            tuple<Nodo*, int> resultado = buscarNodoNulo(entrada.a, alturaActual + 1);
            // Si se encuentra un nodo nulo en los nodos siguientes, devuelve el resultado
            if (get<0>(resultado) != nullptr) {
                return resultado;
            }
        }
    }
    // Si ninguna entrada tiene un puntero nulo, retorna nulo y la altura actual
    return make_tuple(nullptr, alturaActual);
}
