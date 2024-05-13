#include "mtree_cp.hpp"

#include <cstdlib>
#include <algorithm>
#include <tuple>
#include <climits>
#include <vector>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cstring>
#include <memory>
#include <random>
#include <chrono>
#include <numeric>
#include <fstream>
#include <map>

using namespace std;

// Definición de B pa que el compilador no reclame BORRAR DESPUÉS ***********************
int B = 4096 / sizeof(Entrada);
int b = 0.5*B;

// Paso 2: Selección de muestras y asignación inicial
void asignarMuestrasYPuntos(vector<Punto>& P, map<Punto, vector<Punto>>& grupos, int B) {
    int n = P.size();
    int k = min(B, static_cast<int>(ceil(static_cast<double>(n) / B)));
    
    // Seleccionar k muestras aleatoriamente
    for (int i = 0; i < k; ++i) {
        int idx = rand() % n;  // Seleccionar índice aleatorio
        grupos[P[idx]] = vector<Punto>();  // Asignar un vector vacío para cada muestra
    }

    // Asignar cada punto a la muestra más cercana
    for (const auto& punto : P) {
        double minDist = numeric_limits<double>::max();
        Punto minSample(0.0, 0.0);
        for (const auto& sample : grupos) {
            double dist = distanciaEuclidiana(punto, sample.first);
            if (dist < minDist) {
                minDist = dist;
                minSample = sample.first;
            }
        }
        grupos[minSample].push_back(punto);
    }
}

// Paso 3: Redistribución de puntos para asegurar que cada grupo tenga al menos b puntos
void redistribuirGrupos(map<Punto, vector<Punto>>& grupos, int b) {
    // Identificar grupos que necesitan más puntos
    vector<Punto> muestrasDeficitarias;
    for (const auto& grupo : grupos) {
        if (grupo.second.size() < b) {
            muestrasDeficitarias.push_back(grupo.first);
        }
    }

    // Redistribuir puntos desde grupos cercanos con suficientes puntos
    for (const Punto& muestra : muestrasDeficitarias) {
        double minDist = numeric_limits<double>::max();
        Punto muestraDonante(0.0, 0.0);

        for (const auto& grupo : grupos) {
            if (grupo.second.size() > b && grupo.first != muestra) {
                double dist = distanciaEuclidiana(muestra, grupo.first);
                if (dist < minDist && grupo.second.size() > b + (b / 2)) {
                    minDist = dist;
                    muestraDonante = grupo.first;
                }
            }
        }

        // Realizar la redistribución
        while (grupos[muestra].size() < b && grupos[muestraDonante].size() > b) {
            grupos[muestra].push_back(grupos[muestraDonante].back());
            grupos[muestraDonante].pop_back();
        }
    }
}

// Paso 4: 
void dividirNodo(Nodo* nodo, int maxCapacidad) {
    if (!nodo->entradas.size() > maxCapacidad) return;

    // Ordenar las entradas basadas en alguna métrica, por ejemplo, el valor x del punto
    sort(nodo->entradas.begin(), nodo->entradas.end(), [](const Entrada& a, const Entrada& b) {
        return a.p.x < b.p.x;  // Ordenamos basándonos en la coordenada x
    });

    // Dividir el nodo en dos mitades
    size_t midPoint = nodo->entradas.size() / 2;
    vector<Entrada> leftEntries(nodo->entradas.begin(), nodo->entradas.begin() + midPoint);
    vector<Entrada> rightEntries(nodo->entradas.begin() + midPoint, nodo->entradas.end());

    // Crear dos nuevos nodos para las mitades
    Nodo* leftNode = new Nodo(true);  // Asumiendo que son hojas
    Nodo* rightNode = new Nodo(true); // Asumiendo que son hojas
    leftNode->entradas = leftEntries;
    rightNode->entradas = rightEntries;

    // Limpiar el nodo actual y convertirlo en un nodo interno si no lo es
    nodo->entradas.clear();
    nodo->entradas.push_back(Entrada(leftNode->entradas[0].p, 0.0, leftNode));
    nodo->entradas.push_back(Entrada(rightNode->entradas[0].p, 0.0, rightNode));
}

// Función para manejar la división de nodos y asegurar que no se exceda la capacidad
void balancearArbol(Nodo* raiz, int B) {
    if (raiz->necesitaDivision(B)) {
        dividirNodo(raiz, B);
    }
    // Si el nodo es interno, aplicar balanceo de forma recursiva
    if (!raiz->esHoja) {
        for (auto& entrada : raiz->entradas) {
            balancearArbol(entrada.a, B);
        }
    }
}

void verificarYOptimizarRaiz(Nodo*& raiz) {
    // Verificar si la raíz tiene un único hijo y actualizar la raíz
    if (!raiz->esHoja && raiz->entradas.size() == 1) {
        Nodo* nuevoRaiz = raiz->entradas[0].a;
        delete raiz;  // Eliminar la antigua raíz
        raiz = nuevoRaiz;  // Actualizar la raíz
    }
}

// Paso 5
void ajustarRadiosCobertores(Nodo* nodo) {
    if (nodo->esHoja) return;  // Los nodos hoja no necesitan ajuste de radios

    for (auto& entrada : nodo->entradas) {
        ajustarRadiosCobertores(entrada.a);  // Ajustar recursivamente
        entrada.cr = 0;  // Recalcular el radio cobertor
        for (const auto& subEntrada : entrada.a->entradas) {
            double dist = distanciaEuclidiana(entrada.p, subEntrada.p) + subEntrada.cr;
            entrada.cr = max(entrada.cr, dist);
        }
    }
}

void finalizarConstruccionMTree(Nodo*& raiz) {
    verificarYOptimizarRaiz(raiz);
    ajustarRadiosCobertores(raiz);
}


void construirSubArboles(Nodo* nodo, int B) {
    if (nodo->esHoja) return;  // Si es una hoja, no hay más que hacer aquí

    // Recursivamente asegurar que cada hijo esté correctamente estructurado
    for (auto& entrada : nodo->entradas) {
        if (entrada.a->necesitaDivision(B)) {
            dividirNodo(entrada.a, B);
        }
        construirSubArboles(entrada.a, B);  // Construcción recursiva
    }

    // Reajustar el radio cobertor tras las posibles modificaciones
    ajustarRadiosCobertores(nodo);
}

// Paso 8: Determinar la altura mínima de los árboles Tj
void balancearAlturas(vector<Nodo*>& subarboles, vector<Nodo*>& Tprima) {
    int h = INT_MAX;
    for (auto nodo : subarboles) {
        int alturaNodo = calcularAltura(nodo);
        h = min(h, alturaNodo);
    }

    for (auto nodo : subarboles) {
        if (calcularAltura(nodo) == h) {
            Tprima.push_back(nodo);
        }
    }
}

// Paso 9: Revisión de T' y ajuste de subárboles
void revisarYajustarSubarboles(MTree& T, int h) {
    vector<Nodo*> subarbolesAdecuados;
    vector<Entrada> entradas; // Usadas para mantener una referencia a las entradas relevantes

    for (auto& entrada : T.raiz->entradas) {
        vector<Nodo*> subarbolesTemp;
        encontrarSubarboles(entrada, h, subarbolesTemp, entradas);
        subarbolesAdecuados.insert(subarbolesAdecuados.end(), subarbolesTemp.begin(), subarbolesTemp.end());
    }

    // balancearAltura(subarbolesAdecuados, h); // Potencialmente ajusta o reconfigura subárboles
}

// Declaración adelantada de la función
MTree metodoCP(vector<Punto>& P);

// Paso 10: Construir T^sup aplicando CP a F
MTree construirTSup(vector<Punto>& F) {
    return metodoCP(F);  // Reutilizamos el método CP para construir el árbol superior
}

// Paso 11: Unir subárboles Tj a Tsup
void unirSubarbolesATsup(MTree& Tsup, const vector<Nodo*>& Tj, const map<Punto, Nodo*>& mapeoPfj) {
    for (auto& nodo : Tj) {
        // Asumimos que tenemos un mapeo de pfj y extraemos el Nodo* correspondiente al punto de entrada del nodo
        Nodo* nodoCorrespondiente = mapeoPfj.at(nodo->entradas.front().p);
        
        // Encontrar la hoja correspondiente en Tsup y unir el subárbol
        Nodo* hoja = buscarHoja(Tsup.raiz, nodo->entradas.front().p);
        if (hoja && hoja->esHoja) {
            hoja->esHoja = false;  // Transformar la hoja en un nodo interno si es necesario
            hoja->entradas.push_back(Entrada(nodo->entradas.front().p, 0, nodoCorrespondiente));  // Unir el nodo
        }
    }
}

// Paso 12: Ajustar radios cobertores
// actualizarCR en hpp


// Función principal del método Ciaccia-Patella (CP)
MTree metodoCP(vector<Punto>& P) {

    std::vector<Entrada> entradas;
    if (P.size() <= B) {
        Nodo* raiz = new Nodo();
        for (const Punto& p : P) {
            raiz->agregarPunto(p); // Asegúrate de que agregarPunto esté correctamente definido
        }
        return MTree(raiz);
    }

    map<Punto, vector<Punto>> grupos;
    random_device rd;
    mt19937 g(rd());
    shuffle(P.begin(), P.end(), g);

    // Paso 2: Selección de muestras y asignación inicial
    int k = min(B, static_cast<int>(ceil(double(P.size()) / B)));
    for (int i = 0; i < k; ++i) {
        grupos[P[i]] = {};
    }

    // Paso 3: Asignación de puntos a las muestras
    for (const Punto& punto : P) {
        double minDist = numeric_limits<double>::max();
        Punto minSample = P[0];  // Asumir al menos un punto existe
        for (const auto& sample : grupos) {
            double dist = distanciaEuclidiana(punto, sample.first);
            if (dist < minDist) {
                minDist = dist;
                minSample = sample.first;
            }
        }
        grupos[minSample].push_back(punto);
    }

    // Paso 3: Redistribución si es necesario
    redistribuirGrupos(grupos, b);

    // Paso 4-7: Construcción y unión de subárboles
    Nodo* raiz = new Nodo();
    for (const auto& grupo : grupos) {
        vector<Punto> subPuntos = grupo.second;
        MTree subTree = metodoCP(subPuntos);  // Llamada recursiva
        Nodo* subTreeNode = subTree.raiz;
        raiz->agregarEntrada(grupo.first, 0.0, subTreeNode);  // Ajustar radio cobertor después
    }

    // Paso 8-12: Finalización y ajustes
    finalizarConstruccionMTree(raiz);

    return MTree(raiz);
}

