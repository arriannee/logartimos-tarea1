#include "mtree.hpp"
#include <cmath>

// Función para calcular la distancia euclidiana entre dos puntos.
double distanciaEuclidiana(Punto p1, Punto p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}