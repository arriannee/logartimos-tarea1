#include "mtree.hpp"
#include "mtree.cpp"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <tuple>
#include <limits>
using namespace std;

// Definición de B pa que el compilador no reclame BORRAR DESPUÉS ***********************
int B = 3141592;
int b = 0.5*B;


// Método Ciaccia-Patella (CP)
MTree* metodoCP(const vector<Punto>& P) {
    int n = P.size();
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
    vector<Punto> F; // Vector F de samples

    // Vector que almacena los F1, ... , Fk
    vector<tuple<vector<Punto>, Punto>> F_l;

    do {
        // Paso 2: De manera aleatoria se eligen k = min(B, n/B) puntos de P, que los llamaremos samples pf1, ..., pfk. Se insertan en un conjunto F de samples
        int k = min(B, n/B);  // Número de samples
        

        for (int i = 0; i < k; i++) {
            Punto pfi = P[rand() % n]; // Se elijen samples aleatorios
            F.push_back(pfi); // Se inserta el sample pfi en F
        }

        // Paso 3: Se le asigna a cada punto en P su sample más cercano. Con eso se puede construir k conjuntos F1, . . . , Fk
        
        // Se crean los conjuntos Fi pertenecientes a F_l
        for (Punto pfi : F) {
            vector<Punto> Fi;  // Conjunto que almacena los Puntos tales que pfi es más cercano
            tuple<vector<Punto>, Punto> tupla(Fi, pfi);
            F_l.push_back(tupla);
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
            for (auto tupla : F_l){
                if (get<1>(tupla).x == puntoMasCercano.x && get<1>(tupla).y == puntoMasCercano.y){
                    get<0>(tupla).push_back(puntoP);
                    break;
                }
            }
        }

        // Paso 4: Etapa de redistribución: Si algún Fj es tal que |Fj |< b:
        // 4.1 Quitamos pfj de F
        for (auto tupla : F_l){
            if (get<0>(tupla).size() < b) {
                Punto puntoABuscar = get<1>(tupla);
                F.erase(std::remove_if(F.begin(), F.end(), [&puntoABuscar](const Punto& punto){ 
                    return punto.x == puntoABuscar.x && punto.y == puntoABuscar.y; 
                }), F.end());
            }
            // 4.2 Por cada p ∈ Fj , le buscamos el sample pfl más cercano de F y lo añadimos a su conjunto Fl
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
                for (auto tupla : F_l){
                    if (get<1>(tupla).x == puntoMasCercano.x && get<1>(tupla).y == puntoMasCercano.y){
                        get<0>(tupla).push_back(puntoFj);
                        break;
                    }
                }
            }
        }    
    } while (F.size() == 1); // Paso 5: Si |F|= 1, volver al paso 2.
    
    vector<MTree> subarboles;
    // Paso 6: Se realiza recursivamente el algoritmo CP en cada Fj, obteniendo el arbol Tj
    for (auto tupla : F_l) {
        MTree* Tj = metodoCP(get<0>(tupla));

        // Paso 7: Verificar si la raíz del árbol Tj es de un tamaño menor a b
        if (Tj->raiz->entradas.size() < b) {
            std::vector<std::tuple<MTree*, Punto>> subarboles_tuplas;
        
            for (auto& entrada : Tj->raiz->entradas) {
                std::vector<Entrada> entradas_hijo;
                if (entrada.a != nullptr) { // Si no es una hoja
                    entradas_hijo = entrada.a->entradas;
                }
                Nodo* nuevo_nodo = new Nodo(entradas_hijo, B);
                MTree* nuevo_arbol = new MTree(nuevo_nodo);
                subarboles_tuplas.push_back(std::make_tuple(nuevo_arbol, entrada.p)); // Almacenamos el árbol con su punto asociado
            }
        
            delete Tj->raiz; // Eliminar la raíz actual
            Tj->raiz = nullptr;
        
            // Reasignar lo puntos a los nueos subarborles
            for (auto& tupla : F_l) {
                for (Punto& punto : get<0>(tupla)) {
                    int indiceMasCercano = 0;
                    double distanciaMin = std::numeric_limits<double>::max();
        
                    // aca bsuca el sub arbol ams cerca no y se lo asigna
                    for (int k = 0; k < subarboles_tuplas.size(); k++) {
                        double distanciaActual = distanciaEuclidiana(punto, std::get<1>(subarboles_tuplas[k]));
                        if (distanciaActual < distanciaMin) {
                            indiceMasCercano = k;
                            distanciaMin = distanciaActual;
                        }
                    }
                    std::get<0>(subarboles_tuplas[indiceMasCercano])->raiz->agregarPunto(punto);
                }
            }
        
            // Limpieza: Eliminar el punto de F que fue usado como raíz en Tj
            // esta wea se la pase a chat gpt por que no sabia como elimar ese putno con las tuplas era mas faccil con el Begin(erase()+i) que tenai antes
            F.erase(std::remove_if(F.begin(), F.end(), [&](const Punto& punto) {
                return punto.x == get<1>(F_l[i]).x && punto.y == get<1>(F_l[i]).y;
            }), F.end());
        }


    
};
