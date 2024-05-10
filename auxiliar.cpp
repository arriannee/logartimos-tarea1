#include <limits>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "mtree.hpp"

// Calcula la distancia entre dos puntos p1 y p2
double distanciaEuclidiana(Punto p1, Punto p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// Recibe un Punto p y un vector de Punto V, si p está efectivamente en V, lo elimina
void borrarPuntoDeVector(Punto p, vector<Punto> V) {
	V.erase(remove_if(V.begin(), V.end(), [&p](const Punto& punto){ 
                    return punto.x == p.x && punto.y == p.y; 
                }), V.end());
}

// Recibe un nodo (la raíz de un MTree) y calcula su altura
int altura(Nodo* nodo){
	if (nodo == nullptr){
		return 0;
	}
	int alturaMax = 0;
    for (Entrada entrada : nodo->entradas) {
        alturaMax = max(alturaMax, altura(entrada.a));
    }
    
    return alturaMax + 1;
}


