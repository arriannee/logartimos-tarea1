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

#include <optional>

#include "mtree.hpp"

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
        auto [idx1, idx2] = encontrarParesMasCercanos(C);
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
    Nodo *nodo = new Nodo(entradas, cluster.puntos.size());

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
        entradas.emplace_back(g, r, reinterpret_cast<Entrada*>(a));
    }

    // Paso 4: Crear el nodo interno
    Nodo *nodo = new Nodo(entradas, medoides.size());
    
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

// Función para realizar las consultas
int realizarConsulta(Nodo* root, Punto q, double radio, int& contadorAccesos) {
    int accesos = 0;
    if (root == nullptr) return accesos;

    std::vector<Nodo*> nodosPorVerificar = {root};
    while (!nodosPorVerificar.empty()) {
        Nodo* nodo = nodosPorVerificar.back();
        nodosPorVerificar.pop_back();
        ++accesos;
        ++contadorAccesos;  // Incrementar el contador de accesos

        for (const auto& entrada : nodo->entradas) {
            if (distanciaEuclidiana(q, entrada.p) <= radio + entrada.cr) {
                if (entrada.a != nullptr) {
                    nodosPorVerificar.push_back(reinterpret_cast<Nodo*>(entrada.a));
                }
            }
        }
    }
    return accesos;
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
    int B = 4096 / sizeof(Entrada); // Calcular el tamaño máximo del bloque
    int b = B / 2; // Calcular el tamaño mínimo del bloque

    std::vector<int> n_values;
    for (int i = 10; i <= 25; ++i) {
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
        Nodo* rootSS = SS(P, b, B);
        auto endSS = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> tiempoConstruccionSS = endSS - startSS;
        std::cout << "Tiempo de construccion SS para n=" << n << ": " << tiempoConstruccionSS.count() << " segundos" << std::endl;

        // Evaluar consultas
        std::vector<int> accesosSS;
        int contadorAccesosSS = 0;
        for (const auto& q : Q) {
            int accesos = realizarConsulta(rootSS, q, radioConsulta, contadorAccesosSS);
            accesosSS.push_back(accesos);
        }

        // Calcular estadísticas para SS
        double mediaSS = std::accumulate(accesosSS.begin(), accesosSS.end(), 0.0) / accesosSS.size();
        double varianzaSS = std::accumulate(accesosSS.begin(), accesosSS.end(), 0.0, [mediaSS](double sum, int x) {
            return sum + (x - mediaSS) * (x - mediaSS);
        }) / accesosSS.size();
        double desviacionEstandarSS = std::sqrt(varianzaSS);

        std::cout << "SS - Media de accesos para n=" << n << ": " << mediaSS << ", Desviacion estandar: " << desviacionEstandarSS << ", Varianza: " << varianzaSS << std::endl;
        std::cout << "SS - Total de accesos para n=" << n << ": " << contadorAccesosSS << std::endl;

        // Imprimir el árbol
        printTree(rootSS);

        // Limpiar
        delete rootSS;
    }

    return 0;
}
