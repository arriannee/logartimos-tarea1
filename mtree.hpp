#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <algorithm>
#include <iostream>
#include <cmath>
using namespace std;

// Clase para un punto
struct Punto {
    double x;
    double y;

    Punto(double _x, double _y) : x(_x), y(_y) {} // Constructor  
};

// Clase para una entrada
struct Entrada {
    Punto p;
    double cr;                   // Radio covertor
    struct Nodo *a;              // Dirección en disco a la página de su hijo

    Entrada(Punto _p, double _cr, Nodo *_a) : p(_p), cr(_cr), a(_a) {} // Constructor  
};

struct Nodo {
    vector<Entrada> entradas;       // Conjunto de entradas
    int B = 4096/sizeof(Entrada);  // Capacidad máxima de entradas en cada nodo

    // Método para añadir un Punto dentro del nodo
    void agregarPunto(Punto punto) {
        Entrada nuevaEntrada(punto, 0, nullptr); // Crear la nueva entrada que se añadirá
        entradas.push_back(nuevaEntrada);        // Añadir la entrada al final del vector
        cout << "Punto agregado: (" << punto.x << ", " << punto.y << ") en Nodo" << endl;
    }   

    Nodo(vector<Entrada>& _entradas, int _B) : entradas(_entradas), B(_B) {} // Constructor
};

struct MTree {
    Nodo *raiz;

    MTree(Nodo *_raiz) : raiz(_raiz) {} // Constructor  
};

// Calcula la distancia entre dos puntos p1 y p2
double distanciaEuclidiana(Punto p1, Punto p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// Recibe un Punto p y un vector de Punto V, si p está efectivamente en V, lo elimina
void borrarPuntoDeVector(Punto p, vector<Punto>& V) {
	V.erase(remove_if(V.begin(), V.end(), [&p](const Punto& punto){ 
                    return punto.x == p.x && punto.y == p.y; 
                }), V.end());
}

// Recibe un nodo (la raíz de un MTree) y calcula su altura
int altura(Nodo* nodo){
    // Si el nodo es nulo, su altura es 0
	if (nodo == nullptr){
		return 0;
	}
    // Si no es nulo, su altura actual es 0
	int alturaMax = 0;
    // Se comiezan a contar sus hijos
    for (Entrada entrada : nodo->entradas) {
        // Se llama recursivamente a altura en su hijo
        // La altura máxima será el máximo entre la actual y la altura entregada por el hijo
        alturaMax = max(alturaMax, altura(entrada.a));
    }
    // Se cuenta el nodo actual
    return alturaMax + 1;
}

 // Recibe una raíz de un árbol y busca exhaustivamente todos sus subárboles de altura h, guardándolos en un vector
void encontrarSubarboles(Entrada entrada, int h, vector<Nodo*>& subarboles, vector<Entrada>& entradas) {
    // Si el nodo es nulo o la altura es menor a h, ningún subarbol puede tener altura igual a h, retornar
    if (entrada.a == nullptr || altura(entrada.a) < h) {
        return;
    }
    // Si la altura del nodo es igual a h, se agrega este nodo a la lista de subárboles
    if (altura(entrada.a) == h) {
        subarboles.push_back(entrada.a);
        entradas.push_back(entrada);
        return;
    }
    // Recorrer todas las entradas del nodo
    for (Entrada entrada2 : entrada.a->entradas) {
        // Si el puntero a la entrada no es nulo, se busca en el subárbol
        if (entrada.a != nullptr) {
            encontrarSubarboles(entrada2, h, subarboles, entradas);
        }
    }
}

// Busca la distancia máxima de una entrada a alguno de sus hijos
double calcularDistanciaMaxima(Entrada inicial) {
    // Si no tiene un nodo hijo
    if (inicial.a == nullptr) {
        // Devuelve 0
        return 0;
    } 
    
    double maxDist = 0;
    // Si tiene un nodo hijo, para cada entrada en su nodo hijo
    for (Entrada entrada : inicial.a->entradas) {
        // Calcula recursivamente la distancia entre la entrada de su nodo hijo con sus nodos hijos
        double extra = calcularDistanciaMaxima(entrada);
        // Suma la distancia total encontrada para su nodo hijo con la propia
        double dist = distanciaEuclidiana(inicial.p, entrada.p) + extra;
        // Se queda con la máxima distancia encontrada
        maxDist = max(dist, maxDist);
    }
    // Devuelve la distancia máxima
    return maxDist;
}

// Función que recorre todo un árbol, actualizando los radios cobertores de cada entrada
void actualizarCR(Nodo* n) {
    // Para cada entrada de n
    for (Entrada entradas : n->entradas) {
        // Si es una hoja, tiene cr = 0
        if (entradas.a == nullptr) {
            entradas.cr = 0;
        }
        // Si es un nodo interno
        else {
            // Se calcula su distancia máxima con cualquier punto de su subárbol
            double nuevoCR = calcularDistanciaMaxima(entradas);
            entradas.cr = nuevoCR;
            // Se llama la función sobre su nodo hijo
            actualizarCR(entradas.a);
        }     
    }
}


