#include "mtree.hpp"
#include "mtree.cpp"
#include <vector>

// Definición de B pa que el compilador no reclame BORRAR DESPUÉS ***********************
int B = 3141592;


// Método Ciaccia-Patella (CP)
MTree* metodoCP(int n, const std::vector<Punto>& P) {
    if (n <= B) {
        MTree* T;
        for (int i = 0; i < n; i++) {
            Punto punto = P[i];
        }
        return T;
    }
};