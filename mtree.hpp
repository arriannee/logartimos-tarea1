#include <vector>
#include <set> 

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
    Entrada *a;                     // Dirección en disco a lapágina de su hijo

    Entrada(Punto _p, double _cr, Entrada *_a) : p(_p), cr(_cr), a(_a) {} // Constructor  
};

struct Nodo {
    std::vector<Entrada> entradas;  // Conjunto de entradas
    int B;                          // Capacidad máxima de entradas en cada nodo

    Nodo(std::vector<Entrada>& _entradas, int _B) : entradas(_entradas), B(_B) {} // Constructor  
};

struct MTree {
    Nodo *raiz;

    MTree(Nodo *_raiz) : raiz(_raiz) {} // Constructor  
};

// Función para calcular la distancia euclidiana entre dos puntos.
double distanciaEuclidiana(Punto p1, Punto p2);