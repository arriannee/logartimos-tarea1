#include "mtree.hpp"
#include "mtree.cpp"
#include "auxiliar.cpp"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <tuple>
#include <climits>
using namespace std;

// Definición de B pa que el compilador no reclame BORRAR DESPUÉS ***********************
int B = 3141592;
int b = 0.5*B;


// Método Ciaccia-Patella (CP)
MTree metodoCP(const vector<Punto>& P) {
    int n = P.size();
    // Paso 1: Si |P| ≤ B, se crea un árbol T , se insertan todos los puntos a T y se retorna T
    if (n <= B) {
        MTree T;
        for (int i = 0; i < n; i++) {
            Punto punto = P[i];
            T.raiz->agregarPunto(punto);
        }
        return T;
    }

    // Creamos el vector F
    vector<Punto> F; // Vector F de samples

    // Vector que almacena los F1, ... , Fk
    vector<tuple<vector<Punto>, Punto>> F_j;

    do {
        // Paso 2: De manera aleatoria se eligen k = min(B, n/B) puntos de P, que los llamaremos samples pf1, ..., pfk. Se insertan en un conjunto F de samples
        int k = min(B, n/B);  // Número de samples
        

        for (int i = 0; i < k; i++) {
            Punto pfi = P[rand() % n]; // Se elijen samples aleatorios
            F.push_back(pfi); // Se inserta el sample pfi en F
        }

        // Paso 3: Se le asigna a cada punto en P su sample más cercano. Con eso se puede construir k conjuntos F1, . . . , Fk
        
        // Se crean los conjuntos Fi pertenecientes a F_j
        for (Punto pfi : F) {
            vector<Punto> Fi;  // Conjunto que almacena los Puntos tales que pfi es más cercano
            tuple<vector<Punto>, Punto> tupla(Fi, pfi);
            F_j.push_back(tupla);
        }
        
        // Para cada elemento de P, se revisa su distancia con cada elemento de F y nos quedamos con la que sea menor    
        for (Punto puntoP : P) {
            // Para comenzar decimos que el más cercano es el primero en F
            int menorDistancia = distanciaEuclidiana(puntoP, *F.begin());
            Punto puntoMasCercano = *F.begin();
            // Ahora revisamos el resto de F
            for (Punto puntoF : F) {
                int dist = distanciaEuclidiana(puntoP, puntoF);
                if (dist < menorDistancia) {
                    menorDistancia = dist;
                    puntoMasCercano = puntoF;
                }
            }
            // Buscamos el pfi donde debemos insertar el puntoP
            for (auto tupla : F_j){
                if (get<1>(tupla).x == puntoMasCercano.x && get<1>(tupla).y == puntoMasCercano.y){
                    get<0>(tupla).push_back(puntoP);
                    break;
                }
            }
        }

        // Paso 4: Etapa de redistribución: Si algún Fj es tal que |Fj |< b:
        // 4.1 Quitamos pfj de F
        for (auto tupla : F_j){
            if (get<0>(tupla).size() < b) {
                Punto puntoABuscar = get<1>(tupla);
                borrarPuntoDeVector(puntoABuscar, F);
            }
            // 4.2 Por cada p ∈ Fj , le buscamos el sample pfl más cercano de F y lo añadimos a su conjunto F_j
            // Esto es, en esencia, repetir el paso 3
            for (Punto puntoFj : get<0>(tupla)) {
                // Para comenzar decimos que el más cercano es el primero en F
                int menorDistancia = distanciaEuclidiana(puntoFj, *F.begin());
                Punto puntoMasCercano = *F.begin();
                // Ahora revisamos el resto de F
                for (Punto puntoF : F) {
                    int dist = distanciaEuclidiana(puntoFj, puntoF);
                    if (dist < menorDistancia) {
                        menorDistancia = dist;
                        puntoMasCercano = puntoF;
                    }
                }
                // Buscamos el pfi donde debemos insertar el puntoP
                for (auto tupla : F_j){
                    if (get<1>(tupla).x == puntoMasCercano.x && get<1>(tupla).y == puntoMasCercano.y){
                        get<0>(tupla).push_back(puntoFj);
                        break;
                    }
                }
            }
        }    
    } while (F.size() == 1); // Paso 5: Si |F|= 1, volver al paso 2.
    
    vector<MTree> Tj;
    // Paso 6: Se realiza recursivamente el algoritmo CP en cada Fj, obteniendo el arbol Tj
    for (auto tupla : F_j) {
        MTree arbolRecursivo = metodoCP(get<0>(tupla));
        // Paso 7: Si la raíz del árbol es de un tamaño menor a b
        if (arbolRecursivo.raiz->entradas.size() < b) {
            for (Entrada entrada : arbolRecursivo.raiz->entradas) {
                // se añaden los puntos pertinentes a F
                F.push_back(entrada.p);
                // se trabaja con sus subárboles como nuevos Tj , . . . , Tj+p−1
                if (entrada.a != nullptr) {
                    MTree subarbol;
                    subarbol.raiz = entrada.a;
                    Tj.push_back(subarbol);
                }
            }
            // Se quita esa raíz, se elimina pfj de F 
            arbolRecursivo.raiz = nullptr;
            Punto pfj = get<1>(tupla);
            borrarPuntoDeVector(pfj, F);
        }
        Tj.push_back(arbolRecursivo);
    }

    // Paso 8: Se define T′ inicialmente como un conjunto vacío.
    vector<MTree> TPrima;
    // Etapa de balanceamiento: Se define h como la altura mínima de los árboles Tj
    int h = INT_MAX;
    for (MTree arbol : Tj) {
        h = min(h, altura(arbol.raiz));
    }

    // Paso 9: 
};
