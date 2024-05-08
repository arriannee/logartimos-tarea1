#include "mtree.hpp"
#include "mtree.cpp"
#include <cstdlib>
#include <vector>
using namespace std;

// Definición de B pa que el compilador no reclame BORRAR DESPUÉS ***********************
int B = 3141592;
int b = 0.5*B;


// Método Ciaccia-Patella (CP)
MTree* metodoCP(const vector<Punto>& P) {
    // Paso 1: Si |P| ≤ B, se crea un árbol T , se insertan todos los puntos a T y se retorna T
    if (P.size() <= B) {
        MTree* T;
        for (int i = 0; i < P.size(); i++) {
            Punto punto = P[i];
            T->raiz->agregarPunto(punto);
        }
        return T;
    }

    // Creamos el vector F
    set<Punto> F; // Vector F de samples

    // Vector que almacena los F1, ... , Fk
    set<tuple<set<Punto>, Punto>> F_l;
    
    tuple<vector<Punto>, Punto> Fi;

    // Paso 5: Si |F|= 1, volver al paso 2.
    while(F.size() <= 1) {
        
        // Paso 2: De manera aleatoria se eligen k = min(B, n/B) puntos de P, que los llamaremos samples pf1, ..., pfk. Se insertan en un conjunto F de samples
        int k = min(B, P.size()/B);  // Número de samples
        

        for (int i = 0; i < k; i++) {
            Punto pfi = P[rand() % P.size()]; // Se elijen samples aleatorios
            F.insert(pfi); // Se inserta el sample pfi en F
        }

        // Paso 3: Se le asigna a cada punto en P su sample más cercano. Con eso se puede construir k conjuntos F1, . . . , Fk
        
        
        // Se crean los conjuntos Fi pertenecientes a F_l
        for (auto i = F.begin(); i != F.end(); i++) {
            Punto aux = *i;
            set<Punto> conjunto;
            conjunto.insert(*i);
            Fi(conjunto, aux);
            F_l.insert(Fi);
        }
        
        // Para cada elemento de P, se revisa su distancia con cada elemento de F y nos quedamos con la que sea menor    
        // Hard-code para encontrar la distancia mínima, propuesto arreglarlo.
        for (int i = 0; i < P.size(); i++) {
            int distanciaMin = distanciaEuclidiana(P[i],*F.begin());
            Punto sampleMasCercano = *F.begin();
            for (auto j = F.begin() ; j != F.end(); j++) {
                int distanciaActual = distanciaEuclidiana(P[i], *j);
                if (distanciaActual < distanciaMin) {
                    sampleMasCercano = *j;
                    distanciaMin = distanciaActual;
                }
            }
            F_l.insert(P[i]);
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
    // Paso 6: Se realiza recursivamente el algoritmo CP en cada Fj, obteniendo el arbol Tj
    for (int i = 0; i < F_l.size(); i++) {
        MTree* Tj = metodoCP(F_l[i].size(), F_l[i]);
        
        // Paso 7: Verificar si la raiz del arbol Tj es de un tamaño menor a b
        if (Tj->raiz->entradas.size() < b) {
            std::vector<MTree*> subarboles;
            for (auto& entrada : Tj->raiz->entradas) {
                std::vector<Entrada> entradas_hijo;
                if (entrada.a != nullptr) { // Si no es una hoja
                    entradas_hijo = entrada.a->entradas;
                }
                Nodo* nuevo_nodo = new Nodo(entradas_hijo, B);
                subarboles.push_back(new MTree(nuevo_nodo));
            }

            delete Tj->raiz; // Eliminar la raiz actual
            Tj->raiz = nullptr;

            // Reasignar los puntos de F_l[i] a los nuevos subarboles
            for (int j = 0; j < F_l[i].size(); j++) {
                int indiceMasCercano = 0;
                double distanciaMin = distanciaEuclidiana(F_l[i][j], subarboles[0]->raiz->entradas[0].p);
                for (int k = 1; k < subarboles.size(); k++) {
                    double distanciaActual = distanciaEuclidiana(F_l[i][j], subarboles[k]->raiz->entradas[0].p);
                    if (distanciaActual < distanciaMin) {
                        indiceMasCercano = k;
                        distanciaMin = distanciaActual;
                    }
                }
                subarboles[indiceMasCercano]->raiz->agregarPunto(F_l[i][j]);
            }

            // Eliminar el punto de F que fue usado como raiz en Tj, aqui tiene el indice del sample
            F.erase(F.begin() + i); //  i es el índice del punto de F que fue raiz
        }
    }

    
};
