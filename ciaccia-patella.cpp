#include "mtree.hpp"
#include <cstdlib>
#include <algorithm>
#include <tuple>
#include <climits>
#include <vector>
using namespace std;

// Definición de B pa que el compilador no reclame BORRAR DESPUÉS ***********************
int B = 4096 / sizeof(Entrada);
int b = 0.5*B;


// Método Ciaccia-Patella (CP)
MTree metodoCP(const vector<Punto>& P) {
    int n = P.size();
    // Paso 1: Si |P| ≤ B, se crea un árbol T , se insertan todos los puntos a T y se retorna T
    if (n <= B) {
        vector<Entrada> entradas;
        for (int i = 0; i < n; i++) {
            Entrada entrada(P[i], 0, nullptr);
            entradas.push_back(entrada);
        }
        Nodo nodo(entradas, 4096/sizeof(nodo));
        Nodo* raiz = &nodo;
        MTree T(raiz);
    }

    // Creamos el vector F
    vector<Punto> F; // Vector F de samples

    // Vector que almacena los F1, ... , Fk junto al sample pfj
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
        // Paso 4.1 Quitamos pfj de F
        for (auto tupla : F_j){
            if (get<0>(tupla).size() < b) {
                Punto puntoABuscar = get<1>(tupla);
                borrarPuntoDeVector(puntoABuscar, F);
            }
            // Paso 4.2 Por cada p ∈ Fj , le buscamos el sample pfl más cercano de F y lo añadimos a su conjunto F_j
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

    vector<tuple<MTree, Punto>> Tj;
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
                    MTree subarbol(entrada.a);
                    tuple<MTree, Punto> tupla(subarbol, entrada.p);
                    Tj.push_back(tupla);
                }
            }
            // Se quita esa raíz, se elimina pfj de F 
            arbolRecursivo.raiz = nullptr;
            Punto pfj = get<1>(tupla);
            borrarPuntoDeVector(pfj, F);
        }
        // Tupla, en la posición 0 se guarda el árbol Tj y en la posición 1 el pfj asociado
        tuple<MTree, Punto> tuplaTj(arbolRecursivo, get<1>(tupla));
        Tj.push_back(tuplaTj);
    }

    // Paso 8: Se define T′ inicialmente como un conjunto vacío.
    vector<MTree> TPrima;
    // Etapa de balanceamiento: Se define h como la altura mínima de los árboles Tj
    int h = INT_MAX;
    for (tuple<MTree, Punto> tupla : Tj) {
        MTree arbolTj = get<0>(tupla);
        h = min(h, altura(arbolTj.raiz));
    }

    // Paso 9: Por cada Tj , si su altura es igual a h, se añade a T′
    for (tuple<MTree, Punto> tupla : Tj) {
        MTree arbolTj = get<0>(tupla);
        if (altura(arbolTj.raiz) == h) {
            TPrima.push_back(arbolTj);
        }
        // Si no se cumple
        else {
            // Paso 9.1: Se borra el punto pertinente en F
            Punto puntoABuscar = get<1>(tupla); // pfj asociado al Fj que genera Tj
            borrarPuntoDeVector(puntoABuscar, F);

            // Paso 9.2: Se hace una búsqueda exhaustiva en Tj de todos los subárboles T′1 , . . . , T′p de altura igual a h
            for (Entrada entrada : arbolTj.raiz->entradas) {
                // Si su altura es igual a h
                if (altura(entrada.a) == h) {
                    MTree TPrimaP(entrada.a);
                    // Se insertan estos árboles a T'
                    TPrima.push_back(TPrimaP);

                    // Paso 9.3: Se insertan los puntos raíz de T′1 , . . . , T′p , pf1', . . . , pfp' en F

                }
                // Si su altura no es igual a h
                else if (altura(entrada.a) < h) {
                    // No hace nada?
                }
            }

        }

            // 

            // Se insertan los puntos raíz de T′1 , . . . , T′p , pf1', . . . , pfp' en F


    }
    // Paso 10: Se define Tsup como el resultado de la llamada al algoritmo CP aplicado a F
    MTree Tsup = metodoCP(F);

    // Paso 11: Se une cada Tj ∈ T′ a su hoja en Tsup correspondiente al punto pfj ∈ F, obteniendo un nuevo árbol T
    //MTree T;

    // Paso 12: Se setean los radios cobertores resultantes para cada entrada en este árbol
    for (Entrada entrada : T.raiz->entradas) {
        entrada.cr = 0;
        for (Entrada entradaSubarbol : entrada.a->entradas) {
            // Aquí pondría mi código si tuviera uno
        }
    }

    // Paso 13: Se retorna T
    //return T;
};
