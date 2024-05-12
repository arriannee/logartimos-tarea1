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
#include <functional>
#include <fstream>

#include "mtree_ss.hpp"

struct Cluster {
    std::vector<Punto> puntos; 
    Punto medoide;
    double radioCobertor = 0.0;

    Cluster(int capacidad = 0) : medoide(0.0, 0.0) {
        puntos.reserve(capacidad);
    }

    int numPuntos() const {
        return puntos.size();
    }

    void agregarPunto(const Punto& punto) {
        puntos.push_back(punto);
    }
};


void calcularMedoide(Cluster& cluster) {
    double minDistancia = std::numeric_limits<double>::infinity();
    int medoideIndex = 0;

    for (int i = 0; i < cluster.numPuntos(); ++i) {
        double sumaDistancias = 0;
        for (int j = 0; j < cluster.numPuntos(); ++j) {
            sumaDistancias += distanciaEuclidiana(cluster.puntos[i], cluster.puntos[j]);
        }
        if (sumaDistancias < minDistancia) {
            minDistancia = sumaDistancias;
            medoideIndex = i;
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

/* DIVDIR Y CONQUISTAR ENCONTRAR PARES MÁS CERCANOS
https://www.geeksforgeeks.org/closest-pair-of-points-onlogn-implementation/
*/

// Función auxiliar para comparar dos entradas basándose en la coordenada X del punto
bool compareX(const Entrada& a, const Entrada& b) {
    return a.p.x < b.p.x;
}

// Función auxiliar para comparar dos entradas basándose en la coordenada Y del punto
bool compareY(const Entrada& a, const Entrada& b) {
    return a.p.y < b.p.y;
}

// Función fuerza bruta (original)
std::pair<int, int> encontrarParesMasCercanos(const std::vector<Cluster>& clusters) {
    double minDistancia = std::numeric_limits<double>::infinity();
    std::pair<int, int> indices{-1, -1};

    for (size_t i = 0; i < clusters.size(); ++i) {
        for (size_t j = i + 1; j < clusters.size(); ++j) {
            double dist = distanciaEntreClusters(clusters[i], clusters[j]);
            if (dist < minDistancia) {
                minDistancia = dist;
                indices = {i, j};
            }
        }
    }

    return indices;
}

// División y conquista para encontrar el par más cercano.
std::pair<int, int> closestPair(std::vector<Cluster>& clusters) {
    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) { // Ordenanmos en orden ascendiente los Clusters
        return a.medoide.x < b.medoide.x;
    });

    // Función recursiva dentro de closestPair para manejar la recursividad (afecta en algo dejarlo aquí dentro?)
    std::function<std::pair<int, int>(int, int)> closestUtil = [&](int left, int right) -> std::pair<int, int> {
        if (right - left <= 3) {
            return encontrarParesMasCercanos(std::vector<Cluster>(clusters.begin() + left, clusters.begin() + right));
        }

        int mid = left + (right - left) / 2;
        auto leftPair = closestUtil(left, mid);
        auto rightPair = closestUtil(mid, right);

        double leftDist = distanciaEntreClusters(clusters[leftPair.first], clusters[leftPair.second]);
        double rightDist = distanciaEntreClusters(clusters[rightPair.first], clusters[rightPair.second]);

        std::pair<int, int> bestPair = leftDist < rightDist ? leftPair : rightPair;
        double bestDist = std::min(leftDist, rightDist);

        // Chequear la franja del medio
        std::vector<Cluster> strip;
        for (int i = left; i < right; ++i) {
            if (std::abs(clusters[i].medoide.x - clusters[mid].medoide.x) < bestDist) {
                strip.push_back(clusters[i]);
            }
        }

        std::sort(strip.begin(), strip.end(), [](const Cluster& a, const Cluster& b) {
            return a.medoide.y < b.medoide.y;
        });

        for (size_t i = 0; i < strip.size(); ++i) {
            for (size_t j = i + 1; j < strip.size() && (strip[j].medoide.y - strip[i].medoide.y) < bestDist; ++j) {
                double dist = distanciaEntreClusters(strip[i], strip[j]);
                if (dist < bestDist) {
                    bestDist = dist;
                    bestPair = {i, j};
                }
            }
        }
        return bestPair;
    };

    return closestUtil(0, clusters.size());
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
void splitMinMaxPolicy(const Cluster& original, Cluster& split1, Cluster& split2) {
    double minMaxRadio = std::numeric_limits<double>::infinity();
    int numPuntos = original.numPuntos();

    for (int i = 0; i < numPuntos; ++i) {
        for (int j = i + 1; j < numPuntos; ++j) {
            Cluster tempSplit1, tempSplit2;
            tempSplit1.puntos.reserve(numPuntos);
            tempSplit2.puntos.reserve(numPuntos);

            // Alternadamente, añadir el punto más cercano a p1 y luego a p2
            for (int k = 0; k < numPuntos; ++k) {
                if (k != i && k != j) {
                    if (distanciaEuclidiana(original.puntos[k], original.puntos[i]) <
                        distanciaEuclidiana(original.puntos[k], original.puntos[j])) {
                        tempSplit1.agregarPunto(original.puntos[k]);
                    } else {
                        tempSplit2.agregarPunto(original.puntos[k]);
                    }
                }
            }

            tempSplit1.agregarPunto(original.puntos[i]);
            tempSplit2.agregarPunto(original.puntos[j]);

            // Calcular el medoide y el radio cobertor para cada split
            calcularMedoide(tempSplit1);
            calcularRadioCobertor(tempSplit1);
            calcularMedoide(tempSplit2);
            calcularRadioCobertor(tempSplit2);

            double maxRadio = std::max(tempSplit1.radioCobertor, tempSplit2.radioCobertor);

            if (maxRadio < minMaxRadio) {
                minMaxRadio = maxRadio;
                split1 = std::move(tempSplit1);
                split2 = std::move(tempSplit2);
            }
        }
    }
}


std::vector<Cluster> cluster(const std::vector<Punto>& Cin, int B) {
    std::vector<Cluster> C, Cout;
    for (const auto& punto : Cin) {
        Cluster cluster(1);
        cluster.puntos.push_back(punto);
        C.push_back(std::move(cluster));
    }

    while (C.size() > 1) {
        auto [idx1, idx2] = closestPair(C);
        if (idx1 == -1 || idx2 == -1) {
            break;
        }

        if (C[idx1].numPuntos() + C[idx2].numPuntos() <= B) {
            C[idx1] = combinarClusters(C[idx1], C[idx2]);
            C.erase(C.begin() + idx2);
        } else {
            Cout.push_back(std::move(C[idx2]));
            C.erase(C.begin() + idx2);
        }
    }

    if (C.size() == 1) {
        if (!Cout.empty()) {
            int idx = encontrarVecinoMasCercano(Cout, C[0]);
            if (idx != -1 && C[0].numPuntos() + Cout[idx].numPuntos() <= B) {
                Cout[idx] = combinarClusters(C[0], Cout[idx]);
            } else {
                Cluster temp;
                splitMinMaxPolicy(C[0], Cout[idx], temp);
                Cout.push_back(std::move(temp));
            }
        } else {
            Cout.push_back(std::move(C[0]));
        }
    }

    return Cout;
}

std::tuple<Punto, double, Nodo*> OutputHoja(const Cluster &cin) {
    // Paso 1: Copiar el cluster de entrada y calcular su medoide y radio cobertor
    Cluster cluster = cin;
    calcularMedoide(cluster);
    calcularRadioCobertor(cluster);

    // Paso 2: Crear las entradas para el nodo hoja
    std::vector<Entrada> entradas;
    for (const auto& punto : cluster.puntos) {
        entradas.emplace_back(punto, 0, nullptr);
    }

    // Paso 3: Crear el nodo hoja
    Nodo *nodo = new Nodo(entradas);

    // Paso 4: Retornar el medoide, radio cobertor y el nodo hoja
    return std::make_tuple(cluster.medoide, cluster.radioCobertor, nodo);
}

std::tuple<Punto, double, Nodo*> OutputInterno(const std::vector<std::tuple<Punto, double, Nodo*>> &cmra) {
    // Paso 1: Obtener todos los medoides del conjunto de tuplas
    std::vector<Punto> medoides;
    for (const auto& [g, r, a] : cmra) {
        medoides.push_back(g);
    }

    // Paso 2: Calcular el medoide y el radio cobertor del conjunto de medoides
    Cluster cluster(medoides.size());
    cluster.puntos = medoides;
    calcularMedoide(cluster);
    calcularRadioCobertor(cluster);

    // Paso 3: Crear las entradas para el nodo interno
    std::vector<Entrada> entradas;
    for (const auto& [g, r, a] : cmra) {
        entradas.emplace_back(g, r, reinterpret_cast<Nodo*>(a));
    }

    // Paso 4: Crear el nodo interno
    Nodo *nodo = new Nodo(entradas);
    
    // Paso 5: Retornar el medoide, radio cobertor y el nodo interno
    return std::make_tuple(cluster.medoide, cluster.radioCobertor, nodo);
}

Nodo* SS(const std::vector<Punto> &Cin, int b, int B) {
    // Paso 1: Manejar el caso base
    if (Cin.size() <= B) {
        // Crear un cluster con capacidad de Cin.size()
        Cluster cinCluster(Cin.size());
        for (const auto &p : Cin) {
            agregarPuntoACluster(cinCluster, p);
        }
        // Crear una hoja a partir del cluster
        auto [g, r, a] = OutputHoja(cinCluster);
        return a;
    }

    // Paso 2: Inicialización
    std::vector<Cluster> C = cluster(Cin, B);
    std::vector<std::tuple<Punto, double, Nodo*>> cmra;

    // Paso 3: Crear nodos hoja
    for (const auto &c : C) {
        cmra.push_back(OutputHoja(c));
    }

    // Paso 4: Crear nodos internos
    while (cmra.size() > static_cast<size_t>(B)) {
        std::vector<Punto> medoides;
        for (const auto &[g, r, a] : cmra) {
            medoides.push_back(g);
        }
        C = cluster(medoides, B);
        std::vector<std::tuple<Punto, double, Nodo*>> nuevaCmra;
        for (const auto &c : C) {
            std::vector<std::tuple<Punto, double, Nodo*>> s;
            for (const auto &[g, r, a] : cmra) {
                bool found = false;
                for (const auto &punto : c.puntos) {
                    if (punto.x == g.x && punto.y == g.y) {
                        found = true;
                        break;
                    }
                }
                if (found) {
                    s.push_back(std::make_tuple(g, r, a));
                }
            }
            nuevaCmra.push_back(OutputInterno(s));
        }
        cmra = std::move(nuevaCmra);
    }

    // Paso 5: Crear nodo raíz
    auto [g, r, a] = OutputInterno(cmra);
    return a;
}
