#include "mtree.hpp"
#include "mtree.cpp"
#include <cstdlib>
#include <vector>

// Definición de B pa que el compilador no reclame BORRAR DESPUÉS ***********************
int B = 3141592;
int b = 0.5*B;


// Método Ciaccia-Patella (CP)
MTree* metodoCP(int n, const std::vector<Punto>& P) {
    // Paso 1: Si |P| ≤ B, se crea un árbol T , se insertan todos los puntos a T y se retorna T
    if (n <= B) {
        MTree* T;
        for (int i = 0; i < n; i++) {
            Punto punto = P[i];
            T->raiz->agregarPunto(punto);
        }
        return T;
    }

    // Creamos el vector F
    std::vector<Punto> F; // Vector F de samples

    // Vector que almacena los F1, ... , Fk
    std::vector<std::vector<Punto>> F_l;

    // Paso 5: Si |F|= 1, volver al paso 2.
    while(F.size() <= 1) {
        
        // Paso 2: De manera aleatoria se eligen k = min(B, n/B) puntos de P, que los llamaremos samples pf1, ..., pfk. Se insertan en un conjunto F de samples
        int k = std::min(B, n/B);  // Número de samples
        

        for (int i = 0; i < k; i++) {
            Punto pfi = P[rand() % n]; // Se elijen samples aleatorios
            F.push_back(pfi); // Se inserta el sample pfi en F
        }

        // Paso 3: Se le asigna a cada punto en P su sample más cercano. Con eso se puede construir k conjuntos F1, . . . , Fk
        for (int i = 0; i < F.size(); i++) {
            std::vector<Punto> Fi;
            F_l.push_back(Fi);
        }
        
        // Para cada elemento de P, se revisa su distancia con cada elemento de F y nos quedamos con la que sea menor    
        // Hard-code para encontrar la distancia mínima, propuesto arreglarlo.
        for (int i = 0; i < P.size(); i++) {
            int distanciaMin = distanciaEuclidiana(P[i],F[0]);
            int sampleMasCercano = 0;
            for (int j = 1; j < F.size(); j++) {
                int distanciaActual = distanciaEuclidiana(P[i], F[j]);
                if (distanciaActual < distanciaMin) {
                    sampleMasCercano = j;
                    distanciaMin = distanciaActual;
                }
            }
            F_l[sampleMasCercano].push_back(P[i]);
        }

        // Paso 4: Etapa de redistribución: Si algún Fj es tal que |Fj |< b:
        // 4.1 Quitamos pfj de F
        for (int j = 0; j < F_l.size(); j++){
            if (F_l[j].size() < b) {
                F.erase(F.begin() + j);
            }
            // 4.2 Por cada p ∈ Fj , le buscamos el sample pfl más cercano de F y lo añadimos a su conjunto Fl
            for (int i = 0; i < F_l[j].size(); i++){
                int distanciaMin = distanciaEuclidiana(F_l[j][i], F[0]);
                int sampleMasCercano = 0;
                for (int k = 1; k < F.size(); k++) {
                    int distanciaActual = distanciaEuclidiana(F_l[j][i], F[k]);
                    if (distanciaActual < distanciaMin && k != j) {
                        sampleMasCercano = k;
                        distanciaMin = distanciaActual;
                    }
                }
                F_l[sampleMasCercano].push_back(F_l[j][i]);
            }
        }
    }
    
    std::vector<MTree> subarboles;
    // Paso 6: Se realiza recursivamente el algoritmo CP en cada Fj , obteniendo el árbol Tj
    for (int i = 0; i < F_l.size(); i++) {
        MTree *Tj = metodoCP(F_l.size(), F_l[i]);
        subarboles.push_back(*Tj);
        if (Tj->raiz->entradas.size() < b){
            
        }
    }

    // Paso 7: Si la raíz del árbol es de un tamaño menor a b, se quita esa raíz, se elimina pfj de F y se trabaja con sus subárboles como nuevos Tj , . . . , Tj+p−1, se añaden los puntos pertinentes a F


    
    
};