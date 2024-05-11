#include <limits>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "mtree.hpp"
/*
// Calcula la distancia entre dos puntos p1 y p2
double distanciaEuclidiana(Punto p1, Punto p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// Recibe un Punto p y un vector de Punto V, si p está efectivamente en V, lo elimina
void borrarPuntoDeVector(Punto p, vector<Punto>& V) {
	V.erase(remove_if(V.begin(), V.end(), [&p](const Punto& punto){ 
                    return punto.x == p.x && punto.y == p.y; 
                }), V.end());
}

// Recibe un nodo (la raíz de un MTree) y calcula su altura
int altura(Nodo* nodo){
    // Si el nodo es nulo, su altura es 0
	if (nodo == nullptr){
		return 0;
	}
    // Si no es nulo, su altura actual es 0
	int alturaMax = 0;
    // Se comiezan a contar sus hijos
    for (Entrada entrada : nodo->entradas) {
        // Se llama recursivamente a altura en su hijo
        // La altura máxima será el máximo entre la actual y la altura entregada por el hijo
        alturaMax = max(alturaMax, altura(entrada.a));
    }
    // Se cuenta el nodo actual
    return alturaMax + 1;
}

 // Recibe una raíz de un árbol y busca exhaustivamente todos sus subárboles de altura h, guardándolos en un vector
void encontrarSubarboles(Nodo* nodo, int h, vector<Nodo*>& subarboles) {
    // Si el nodo es nulo o la altura es menor a h, ningún subarbol puede tener altura igual a h, retornar
    if (nodo == nullptr || altura(nodo) < h) {
        return;
    }
    // Si la altura del nodo es igual a h, se agrega este nodo a la lista de subárboles
    if (altura(nodo) == h) {
        subarboles.push_back(nodo);
        return;
    }
    // Recorrer todas las entradas del nodo
    for (Entrada entrada : nodo->entradas) {
        // Si el puntero a la entrada no es nulo, se busca en el subárbol
        if (entrada.a != nullptr) {
            encontrarSubarboles(entrada.a, h, subarboles);
        }
    }
}

// Busca la distancia máxima de una entrada a alguno de sus hijos
double calcularDistanciaMaxima(Entrada inicial) {
    // Si no tiene un nodo hijo
    if (inicial.a == nullptr) {
        // Devuelve 0
        return 0;
    } 
    
    double maxDist = 0;
    // Si tiene un nodo hijo, para cada entrada en su nodo hijo
    for (Entrada entrada : inicial.a->entradas) {
        // Calcula recursivamente la distancia entre la entrada de su nodo hijo con sus nodos hijos
        double extra = calcularDistanciaMaxima(entrada);
        // Suma la distancia total encontrada para su nodo hijo con la propia
        double dist = distanciaEuclidiana(inicial.p, entrada.p) + extra;
        // Se queda con la máxima distancia encontrada
        maxDist = max(dist, maxDist);
    }
    // Devuelve la distancia máxima
    return maxDist;
}

// Función que recorre todo un árbol, actualizando los radios cobertores de cada entrada
void actualizarCR(Nodo* n) {
    // Para cada entrada de n
    for (Entrada entradas : n->entradas) {
        // Si es una hoja, tiene cr = 0
        if (entradas.a == nullptr) {
            entradas.cr = 0;
        }
        // Si es un nodo interno
        else {
            // Se calcula su distancia máxima con cualquier punto de su subárbol
            double nuevoCR = calcularDistanciaMaxima(entradas);
            entradas.cr = nuevoCR;
            // Se llama la función sobre su nodo hijo
            actualizarCR(entradas.a);
        }     
    }
}
*/

