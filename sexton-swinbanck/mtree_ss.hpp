#include <vector>
#include <set> 
#include <cmath>

// Definir estructura Nodo previamente
struct Nodo;

// Clase para un punto
struct Punto {
    double x;
    double y;

    Punto(double _x, double _y) : x(_x), y(_y) {} // Constructor  
};

// Clase para una entrada
struct Entrada {
    Punto p;
    double cr;                      // Radio covertor
    Nodo *a;                     // Dirección en disco a la página de su hijo

    Entrada(Punto _p, double _cr, Nodo *_a) : p(_p), cr(_cr), a(_a) {} // Constructor  
};

struct Nodo  {
    std::vector<Entrada> entradas;  // Conjunto de entradas
    // int B = 4096 / sizeof(Entrada);  // Capacidad máxima de entradas en cada nodo

    // Método para añadir un Punto dentro del nodo
    void agregarPunto(const Punto& punto) {
        Entrada nuevaEntrada(punto, 0, nullptr); // Crear la nueva entrada que se añadirá
        entradas.push_back(nuevaEntrada);        // Añadir la entrada al final del vector
    }   

   Nodo(std::vector<Entrada>& _entradas) : entradas(_entradas) {} // Constructor  
};

struct MTree {
    Nodo *raiz;

    MTree(Nodo *_raiz) : raiz(_raiz) {} // Constructor  
};

// Función para calcular la distancia euclidiana entre dos puntos.
double distanciaEuclidiana(Punto p1, Punto p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}