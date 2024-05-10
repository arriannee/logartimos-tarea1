#include <vector>
#include <string>
#include <iostream>
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
    int B = 4096/sizeof(entradas);  // Capacidad máxima de entradas en cada nodo

    // Método para añadir un Punto dentro del nodo
    void agregarPunto(Punto punto) {
        Entrada nuevaEntrada(punto, 0, nullptr); // Crear la nueva entrada que se añadirá
        entradas.push_back(nuevaEntrada);        // Añadir la entrada al final del vector
    }   

    Nodo(vector<Entrada>& _entradas, int _B) : entradas(_entradas), B(_B) {} // Constructor
};

struct MTree {
    Nodo *raiz;

    MTree(Nodo *_raiz) : raiz(_raiz) {} // Constructor  
};

// Función para calcular la distancia euclidiana entre dos puntos.
double distanciaEuclidiana(Punto p1, Punto p2);

// Función para borrar un punto de un vector.
void borrarPuntoDeVector(Punto p, vector<Punto> V);

// Función para calcular la altura de un MTree.
int altura(Nodo* nodo);

// Función recursiva para encontrar todos los subárboles de altura h
void encontrarSubarboles(Nodo* nodo, int h, vector<Nodo*>& subarboles);