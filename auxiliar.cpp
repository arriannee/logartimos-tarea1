#include <limits>
#include <algorithm>
#include <vector>
#include <tuple>
#include <iostream>
#include "mtree.hpp"

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