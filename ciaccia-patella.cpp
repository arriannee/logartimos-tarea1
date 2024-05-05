#include "mtree.hpp"
#include "mtree.cpp"
#include <cstdlib>
#include <vector>

// Definición de B pa que el compilador no reclame BORRAR DESPUÉS ***********************
int B = 3141592;


// Método Ciaccia-Patella (CP)
MTree* metodoCP(int n, const std::vector<Punto>& P) {
    // Paso 1: Si |P| ≤ B, se crea un árbol T , se insertan todos los puntos a T y se retorna T

    //NOTAAAAAAAAAAAA cómo se deben agregar exactamente? ************************
    if (n <= B) {
        MTree* T;
        for (int i = 0; i < n; i++) {
            Punto punto = P[i];
            T->raiz->agregarPunto(punto);
        }
        return T;
    }

    // Paso 2: De manera aleatoria se eligen k = min(B, n/B) puntos de P, que los llamaremos samples pf1, ..., pfk. Se insertan en un conjunto F de samples
    int k = (n < B) ? n : B;  // Número de samples
    std::vector<Punto> F; // Vector F de samples

    for (int i = 0; i < k; i++) {
        Punto pfi = P[rand() % n]; // Se elijen samples aleatorios
        F.push_back(pfi); // Se inserta el sample pfi en F
    }

    // ...........
};