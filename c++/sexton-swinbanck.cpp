#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
#include <tuple>

#include <optional>

#include "mtree.hpp"

struct Cluster {
    std::vector<Punto> puntos; 
    Punto medoide;
    double radioCobertor;

    Cluster(int capacidad = 0) : puntos(capacidad), medoide(0, 0) {}

    int numPuntos() const {
        return puntos.size();
    }

    int capacidad() const {
        return puntos.capacity();
    }

    void agregarPunto(const Punto& punto) {
        puntos.push_back(punto); // https://cplusplus.com/reference/vector/vector/push_back/
    }
};

void calcularMedoide(Cluster& cluster) {
    double minDistancia = std::numeric_limits<double>::infinity(); // Para obtener un valor infinito
    double sumaDistancias = 0;
    int medoideIndex = 0;

    for (int i = 0; i < cluster.numPuntos(); ++i) {
        sumaDistancias = 0;
        for (int j = 0; j < cluster.numPuntos(); ++j) {
            sumaDistancias += distanciaEuclidiana(cluster.puntos[i], cluster.puntos[j]);
        }
        if (sumaDistancias < minDistancia) {
            minDistancia = sumaDistancias;
            medoideIndex = static_cast<int>(i);
        }
    }
    cluster.medoide = cluster.puntos[medoideIndex];
}

void calcularRadioCobertor(Cluster& cluster) {
    double maxDist = 0;
    for (const auto& punto : cluster.puntos) {
        double dist = distanciaEuclidiana(cluster.medoide, punto);
        if (dist > maxDist) {
            maxDist = dist;
        }
    }
    cluster.radioCobertor = maxDist;
}

double distanciaEntreClusters(const Cluster& c1, const Cluster& c2) {
    return distanciaEuclidiana(c1.medoide, c2.medoide);
}

int encontrarVecinoMasCercano(const std::vector<Cluster>& clusters, const Cluster& targetCluster) {
    double minDistancia = std::numeric_limits<double>::infinity();
    int indiceVecino = -1;
    for (size_t i = 0; i < clusters.size(); ++i) {
        double dist = distanciaEntreClusters(targetCluster, clusters[i]);
        if (dist < minDistancia && &targetCluster != &clusters[i]) {
            minDistancia = dist;
            indiceVecino = static_cast<int>(i);
        }
    }
    return indiceVecino;
}

std::optional<std::pair<int, int>> encontrarParesMasCercanos(const std::vector<Cluster>& clusters) {
    double minDistancia = std::numeric_limits<double>::infinity();
    std::optional<std::pair<int, int>> indices;

    for (size_t i = 0; i < clusters.size(); ++i) {
        for (size_t j = i + 1; j < clusters.size(); ++j) {
            double dist = distanciaEntreClusters(clusters[i], clusters[j]);
            if (dist < minDistancia) {
                minDistancia = dist;
                indices = std::make_pair(i, j);
            }
        }
    }

    return indices;
}

// Crear un cluster con una capacidad inicial
std::unique_ptr<Cluster> crearCluster(int capacidadInicial) {
    auto cluster = std::make_unique<Cluster>(capacidadInicial);
    return cluster;
}

// dios como amo c++ (borrar comentario xd)
void agregarPuntoACluster(Cluster& cluster, const Punto& punto) {
    cluster.puntos.push_back(punto);
}

Cluster combinarClusters(const Cluster& c1, const Cluster& c2) {
    // Crear el nuevo Cluster con capacidad suficiente
    Cluster nuevo;
    nuevo.puntos.reserve(c1.numPuntos() + c2.numPuntos()); // https://cplusplus.com/reference/vector/vector/reserve/

    // Copiar los puntos de c1 y c2
    nuevo.puntos.insert(nuevo.puntos.end(), c1.puntos.begin(), c1.puntos.end()); // https://cplusplus.com/reference/vector/vector/insert/
    nuevo.puntos.insert(nuevo.puntos.end(), c2.puntos.begin(), c2.puntos.end());

    // Calcular el nuevo medoide
    calcularMedoide(nuevo);

    return nuevo;
}

// Rehacer
void splitMinMaxPolicy(Cluster& original, Cluster& split1, Cluster&& split2, int B) {
    double minMaxRadio = std::numeric_limits<double>::infinity();

    for (int i = 0; i < original.numPuntos(); ++i) {
        for (int j = i + 1; j < original.numPuntos(); ++j) {
            Cluster tempSplit1, tempSplit2;
            tempSplit1.puntos.reserve(original.numPuntos());
            tempSplit2.puntos.reserve(original.numPuntos());

            // Asignar cada punto al grupo más cercano
            for (int k = 0; k < original.numPuntos(); ++k) {
                if (k != i && k != j) {
                    if (distanciaEuclidiana(original.puntos[k], original.puntos[i]) < 
                        distanciaEuclidiana(original.puntos[k], original.puntos[j])) {
                        tempSplit1.agregarPunto(original.puntos[k]);
                    } else {
                        tempSplit2.agregarPunto(original.puntos[k]);
                    }
                }
            }

            // Agregar los centros iniciales a los clusters
            tempSplit1.agregarPunto(original.puntos[i]);
            tempSplit2.agregarPunto(original.puntos[j]);

            // Calcular el medoide y el radio cobertor para cada split
            calcularMedoide(tempSplit1);
            calcularRadioCobertor(tempSplit1);
            calcularMedoide(tempSplit2);
            calcularRadioCobertor(tempSplit2);

            double maxRadio = std::max(tempSplit1.radioCobertor, tempSplit2.radioCobertor);

            // Verificar si esta configuración es la mejor
            if (maxRadio < minMaxRadio) {
                minMaxRadio = maxRadio;
                split1 = std::move(tempSplit1);
                split2 = std::move(tempSplit2);
            }
        }
    }
}

std::vector<Cluster> cluster(const std::vector<Punto>& Cin, int b, int B) {
    // Paso 1: Inicialización de los arreglos de clusters C y Cout
    std::vector<Cluster> C, Cout;
    C.reserve(Cin.size());
    Cout.reserve(Cin.size());

    // Paso 2: Crear un cluster inicial para cada punto
    for (const auto& punto : Cin) {
        Cluster cluster(1); // Cluster con capacidad inicial de 1
        agregarPuntoACluster(cluster, punto);
        C.push_back(std::move(cluster));
    }

    // Paso 3: Proceso de combinación de clusters en C
    while (C.size() > 1) {
        int idx1, idx2;

        // Paso 3.1: Encontrar los dos clusters más cercanos
        auto paresCercanos = encontrarParesMasCercanos(C);
        if (!paresCercanos.has_value()) {
            break;  // Salir si no hay pares para combinar
        }

        idx1 = paresCercanos->first;
        idx2 = paresCercanos->second;

        // Paso 3.2: Combinar clusters si es posible
        if (C[idx1].numPuntos() + C[idx2].numPuntos() <= B) {
            C[idx1] = combinarClusters(C[idx1], C[idx2]); // Combinar los dos clusters
            C.erase(C.begin() + idx2); // Eliminar el segundo cluster
        } else {
            // Mover el cluster que no se puede combinar a Cout
            Cout.push_back(std::move(C[idx2]));
            C.erase(C.begin() + idx2); // Reducir numC ya que un cluster ha sido movido a Cout
        }
    }

    // Paso 4: Manejar el último cluster en C
    if (C.size() == 1) {
        // Paso 5 y 6: Tratar de combinar con un cluster en Cout si es posible
        if (!Cout.empty()) {
            int idx = encontrarVecinoMasCercano(Cout, C[0]);
            if (C[0].numPuntos() + Cout[idx].numPuntos() <= B) {
                Cout[idx] = combinarClusters(C[0], Cout[idx]); // Combinar y actualizar en Cout
            } else {
                // Dividir usando MinMax si la combinación no es posible
                splitMinMaxPolicy(C[0], Cout[idx], b, B);
            }
        } else {
            Cout.push_back(std::move(C[0])); // Simplemente mover a Cout si no hay vecinos
        }
    }

    // Paso 7: Retornar el array de clusters procesados en Cout
    return Cout;
}

// Función OutputHoja
std::tuple<Punto, double, std::unique_ptr<Nodo>> OutputHoja(Cluster& Cin) {
    // Paso 1: Calcular el medoide y el radio cobertor
    calcularMedoide(Cin);  // g es el medoide primario de Cin
    double r = 0;
    std::unique_ptr<Nodo> C = std::make_unique<Nodo>(true, Cin.numPuntos()); // Contiene las entradas del nodo hoja

    // Paso 2: Añadir puntos al nodo hoja
    for (const auto& p : Cin.puntos) {
        C->entradas.emplace_back(p, 0, nullptr);  // Añadir (p, null, null) a C (https://cplusplus.com/reference/vector/vector/emplace_back/)
        r = std::max(r, distanciaEuclidiana(Cin.medoide, p));
    }

    // Paso 3: Guardar el puntero a C como a
    std::unique_ptr<Nodo> a = std::move(C);

    // Paso 4: Retornar la tupla (g, r, a)
    return std::make_tuple(Cin.medoide, r, std::move(C));
}

std::tuple<Punto, double, std::unique_ptr<Nodo>> OutputInterno(const std::vector<std::tuple<Punto, double, std::unique_ptr<Nodo>>>& Cmra) {
    // Paso 1: Crear el conjunto Cin con los puntos 'g' y calcular el medoide G
    std::vector<Punto> Cin;
    for (const auto& cmra : Cmra) {
        Cin.push_back(std::get<0>(cmra));
    }

    Cluster tempCluster;
    tempCluster.puntos = Cin;
    calcularMedoide(tempCluster);
    Punto G = tempCluster.medoide;
    double R = 0;
    auto C = std::make_unique<Nodo>(Cin.size());

    // Paso 2: Añadir (g, r, a) a C y actualizar el radio cobertor R
    for (const auto& cmra : Cmra) {
        const auto& g = std::get<0>(cmra);
        const auto& r = std::get<1>(cmra);

        // malo
        std::unique_ptr<Nodo> a = std::move(C);
        // fin de lo malo

        double distancia = distanciaEuclidiana(G, g);
        C->entradas.emplace_back(g, r, std::move(a));
        R = std::max(R, distancia + r);
    }

    // Paso 3: Guardar el puntero a C como A
    std::unique_ptr<Nodo> A = std::move(C);

    // Paso 4: Retornar la tupla (G, R, A)
    return std::make_tuple(G, R, std::move(A));
}

// Función para construir un M-tree a partir de un conjunto de puntos Cin
std::unique_ptr<Nodo> construirMTree(const std::vector<Punto>& Cin, int B) {
    // Paso 1: Si el tamaño de Cin es menor o igual a B, retornar la hoja correspondiente
    if (Cin.size() <= static_cast<decltype(Cin.size())>(B)) {
        Cluster cluster;
        cluster.puntos = Cin;
        calcularMedoide(cluster);
        calcularRadioCobertor(cluster);
        auto tupla =  OutputHoja(cluster);
        std::unique_ptr<Nodo>& a = std::get<2>(tupla);
        return std::move(a); 
    }

    // Paso 2: Clusterizar los puntos de Cin
    std::vector<Cluster> Cout = cluster(Cin, B, B);

    // Paso 3: Construir el M-tree recursivamente
    std::vector<std::tuple<Punto, double, std::unique_ptr<Nodo>>> C;
    for (auto& cluster : Cout) {
        C.push_back(OutputHoja(cluster));
    }

    while (C.size() > static_cast<decltype(C.size())>(B)) {
        
    }

    // Paso 5: Sea (g, r, a) = OutputInterno(C)
    auto tupla = OutputInterno(C);

    // Paso 6: Se retorna a
    std::unique_ptr<Nodo>& a2 = std::get<2>(tupla);
    return std::move(a2);
}

int main() {
    std::cout << "¡Hola! Este es un mensaje llamativo para ayudarte a ver dónde estás en la terminal (me voy a volver loca)." << std::endl;
    return 0;
}