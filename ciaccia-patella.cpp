#include "mtree.hpp"
#include <cstdlib>
#include <algorithm>
#include <tuple>
#include <climits>
#include <vector>

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
#include <tuple>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <fstream>
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
        Nodo* raiz = new Nodo(entradas, 4096/sizeof(Entrada));
        for (const auto& punto : P) {
            raiz->agregarPunto(punto);
        }
        return MTree(raiz);
    }

    // Creamos el vector F
    vector<Punto> F; // Vector F de samples

    // Vector que almacena los F1, ... , Fk junto al sample pfj
    vector<tuple<vector<Punto>, Punto>> F_j;

    do {
        vector<Punto> F;
        vector<tuple<vector<Punto>, Punto>> F_j;
        // Paso 2: De manera aleatoria se eligen k = min(B, n/B) puntos de P, que los llamaremos samples pf1, ..., pfk. Se insertan en un conjunto F de samples
        int k = int(ceil(min(double(B), double(n)/B)));  // Número de samples
        

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
            for (Punto &puntoF : F) {
                int dist = distanciaEuclidiana(puntoP, puntoF);
                if (dist < menorDistancia) {
                    menorDistancia = dist;
                    puntoMasCercano = puntoF;
                }
            }
            // Buscamos el pfi donde debemos insertar el puntoP
            for (auto &tupla : F_j){
                if (get<1>(tupla).x == puntoMasCercano.x && get<1>(tupla).y == puntoMasCercano.y){
                    get<0>(tupla).push_back(puntoP);
                    break;
                }
            }
        }

        // Paso 4: Etapa de redistribución: Si algún Fj es tal que |Fj |< b:
        // Paso 4.1 Quitamos pfj de F
        for (auto &tupla : F_j){
            if (get<0>(tupla).size() < b) {
                Punto puntoABuscar = get<1>(tupla);
                borrarPuntoDeVector(puntoABuscar, F);
            }
            // Paso 4.2 Por cada p ∈ Fj , le buscamos el sample pfl más cercano de F y lo añadimos a su conjunto F_j
            // Esto es, en esencia, repetir el paso 3
            for (Punto &puntoFj : get<0>(tupla)) {
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
                for (auto &tupla : F_j){
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
    for (auto &tupla : F_j) {
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
        } else {
            // Tupla, en la posición 0 se guarda el árbol Tj y en la posición 1 el pfj asociado
            tuple<MTree, Punto> tuplaTj(arbolRecursivo, get<1>(tupla));
            Tj.push_back(tuplaTj);
        }       
    }

    // Paso 8: Se define T′ inicialmente como un conjunto vacío.
    vector<MTree> TPrima;
    // Etapa de balanceamiento: Se define h como la altura mínima de los árboles Tj
    int h = INT_MAX;
    for (tuple<MTree, Punto> tupla : Tj) {
        MTree arbolTj = get<0>(tupla);
        h = min(h, altura(arbolTj.raiz));
    }

    vector<MTree> Borrar; // Los Tj asociados a pfj que se deben eliminar 
    
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
            vector<Nodo*> busqueda; // En este vector se almacenarán todos los subarboles de altura h
            vector<Entrada> raices; // En este vector se alacenarán los nodos raíces de dichos subárboles
            for (Entrada entrada : arbolTj.raiz->entradas) {
                encontrarSubarboles(entrada, h, busqueda, raices);
            }
            // Se insertan estos árboles a T'
            for (Nodo* nodo : busqueda){
                MTree TPrima_i(nodo); // Se crean los árboles T′1 , . . . , T′p (T'_i)
                TPrima.push_back(TPrima_i);
            // Paso 9.3: Se insertan los puntos raíz de T′1 , . . . , T′p , pf1', . . . , pfp' en F
            for (Entrada raiz : raices) {
                // entrada.p son los puntos raíz de T'i
                F.push_back(raiz.p);
            }
            }
        }

    }

    // Paso 10: Se define Tsup como el resultado de la llamada al algoritmo CP aplicado a F
    MTree Tsup = metodoCP(F);

    // Paso 11: Se une cada Tj ∈ T′ a su hoja en Tsup correspondiente al punto pfj ∈ F
    for (Entrada entradaTsup : Tsup.raiz->entradas) {   // Para cada entrada de Tsup
        for (tuple<MTree, Punto> tuplaTj : Tj) {        // Para cada Tj
            // El punto de la entrada es igual al pfj asignado a Tj, es su hoja correspondiente en Tsup
            if (entradaTsup.p.x == get<1>(tuplaTj).x && entradaTsup.p.y == get<1>(tuplaTj).y) {
                Nodo* raizTj = get<0>(tuplaTj).raiz;
                // La entrada apunta a la raíz de Tj (se unen)
                entradaTsup.a = raizTj;
            }
        }
    }
    // obteniendo un nuevo árbol T
    MTree T = Tsup;

    // Paso 12: Se setean los radios cobertores resultantes para cada entrada en este árbol
    actualizarCR(T.raiz);

    // Paso 13: Se retorna T
    return T;
};

// Función para realizar las consultas
std::pair<int, int> query(const MTree& tree, Punto q, double radio) {
    int accesos = 0; // Contador de accesos
    int puntosEncontrados = 0; // Contador de puntos encontrados
    if (tree.raiz == nullptr) return {accesos, puntosEncontrados};

    std::vector<Nodo*> nodosPorVerificar = {tree.raiz};
    while (!nodosPorVerificar.empty()) { // Mientras hayan Nodos por verificar
        Nodo* nodo = nodosPorVerificar.back();
        nodosPorVerificar.pop_back();
        ++accesos;

        for (const auto& entrada : nodo->entradas) { // Realizar consulta con el radio y el punto
            if (distanciaEuclidiana(q, entrada.p) <= radio + entrada.cr) {
                if (entrada.a != nullptr) {
                    nodosPorVerificar.push_back(reinterpret_cast<Nodo*>(entrada.a));
                } else {
                    ++puntosEncontrados;
                }
            }
        }
    }
    return {accesos, puntosEncontrados};
}


void printTree(Nodo* nodo, int nivel = 0, int hijo = 0) {
    if (nodo == nullptr) return;

    std::string indent(nivel * 2, ' ');

    if (nivel == 0) {
        std::cout << "Entries Root Node:\n";
    } else {
        std::cout << indent << "Entries son " << hijo << " Node:\n";
    }

    for (int i = 0; i < nodo->entradas.size(); ++i) {
        const auto& entrada = nodo->entradas[i];
        std::cout << indent << "  (" << entrada.p.x << ", " << entrada.p.y << ")\n";
        if (entrada.a != nullptr) {
            printTree(reinterpret_cast<Nodo*>(entrada.a), nivel + 1, i);
        }
    }
}


// Experimento para n = 2^10, 2^11, ..., 2^25
int main() {
    std::ofstream outfile("mtree_experiment_output.txt");

    // int B = 4096 / sizeof(Entrada); // Calcular el tamaño máximo del bloque
    // int b = B / 2; // Calcular el tamaño mínimo del bloque

    std::vector<int> n_values;
    for (int i = 8; i <= 8; ++i) {
        n_values.push_back(1 << i);
    }

    double radioConsulta = 0.02;
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> distribucion(0.0, 1.0);
    
    for (int n : n_values) {
        std::vector<Punto> P, Q;
        for (int i = 0; i < n; ++i) {
            P.push_back({distribucion(gen), distribucion(gen)});
        }
        for (int i = 0; i < 100; ++i) {
            Q.push_back({distribucion(gen), distribucion(gen)});
        }

        // Construir el árbol con SS
        auto startSS = std::chrono::high_resolution_clock::now();
        // Nodo* rootSS = metodoCP(P); // Obtengo el Nodo raíz
        MTree treeCP = metodoCP(P); // Creo su árbol con esta raíz
        auto endSS = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tiempoConstruccionSS = endSS - startSS;
        outfile << "Tiempo de construcción SS para n=" << n << ": " << tiempoConstruccionSS.count() << " segundos" << std::endl;

        // Evaluar consultas
        std::vector<int> accesosSS;
        std::vector<int> puntosEncontradosSS;
        auto startConsulta = std::chrono::high_resolution_clock::now();
        for (const auto& q : Q) {
            auto resultados = query(treeCP, q, radioConsulta);
            int accesos = resultados.first;
            int puntosEncontrados = resultados.second;
            accesosSS.push_back(accesos);
            puntosEncontradosSS.push_back(puntosEncontrados);
        }
        auto endConsulta = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tiempoConsultaSS = endConsulta - startConsulta;
        outfile << "Tiempo de consultas SS para n=" << n << ": " << tiempoConsultaSS.count() << " segundos" << std::endl;

        // Calcular estadísticas para SS:
        // Accesos
        double mediaSS = std::accumulate(accesosSS.begin(), accesosSS.end(), 0.0) / accesosSS.size();
        double varianzaSS = std::accumulate(accesosSS.begin(), accesosSS.end(), 0.0, [mediaSS](double sum, int x) {
            return sum + (x - mediaSS) * (x - mediaSS);
        }) / accesosSS.size();
        double desviacionEstandarSS = std::sqrt(varianzaSS);

        // Puntos
        double mediaPuntosEncontradosSS = std::accumulate(puntosEncontradosSS.begin(), puntosEncontradosSS.end(), 0.0) / puntosEncontradosSS.size();
        double varianzaPuntosEncontradosSS = std::accumulate(puntosEncontradosSS.begin(), puntosEncontradosSS.end(), 0.0, [mediaPuntosEncontradosSS](double sum, int x) {
            return sum + (x - mediaPuntosEncontradosSS) * (x - mediaPuntosEncontradosSS);
        }) / puntosEncontradosSS.size();
        double desviacionEstandarPuntosEncontradosSS = std::sqrt(varianzaPuntosEncontradosSS);

        outfile << "CP - Media de accesos para n=" << n << ": " << mediaSS 
                << ", Desviación estándar: " << desviacionEstandarSS 
                << ", Varianza: " << varianzaSS << std::endl;
        outfile << "CP - Media de puntos encontrados para n=" << n << ": " << mediaPuntosEncontradosSS 
                << ", Desviación estándar: " << desviacionEstandarPuntosEncontradosSS 
                << ", Varianza: " << varianzaPuntosEncontradosSS << std::endl;
        
        // Imprimir el árbol
        printTree(treeCP.raiz);

        // Limpiar
        //delete treeCP;
    }

    outfile.close();
    return 0;
}

