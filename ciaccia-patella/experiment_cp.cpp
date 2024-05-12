#include "ciaccia-patella.hpp"

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
    std::ofstream outfile("ciaccia_patella_output.txt");

    int B = 4096 / sizeof(Entrada); // Calcular el tamaño máximo del bloque
    int b = B / 2; // Calcular el tamaño mínimo del bloque

    std::vector<int> n_values;
    for (int i = 10; i <= 25; ++i) {
        n_values.push_back(1 << i);
    }

    double radioConsulta = 0.02;
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> distribucion(0.0, 1.0);


    // Generar Puntos (P) y Queries (Q)
    for (int n : n_values) {
        std::vector<Punto> P, Q;
        for (int i = 0; i < n; ++i) {
            P.push_back({distribucion(gen), distribucion(gen)});
        }
        for (int i = 0; i < 100; ++i) {
            Q.push_back({distribucion(gen), distribucion(gen)});
        }

        // Construir el árbol con CP
        auto startCP = std::chrono::high_resolution_clock::now(); // Comenzar a medir el tiempo de construcción
        MTree treeCP = metodoCP(P); // Creo su árbol con estos Puntos
        auto endCP = std::chrono::high_resolution_clock::now(); // Detener medición del tiempo de construcción
        std::chrono::duration<double> tiempoConstruccionCP = endCP - startCP;
        outfile << "Tiempo de construcción CP para n=" << n << ": " << tiempoConstruccionCP.count() << " segundos" << std::endl;

        // Evaluar consultas
        std::vector<int> accesosCP;
        std::vector<int> puntosEncontradosCP;
        std::vector<double> tiemposConsultaCP;

        for (const auto& q : Q) {
            auto startConsulta = std::chrono::high_resolution_clock::now(); // Comenzar a medir el tiempo de búsqueda
            auto [accesos, puntosEncontrados] = query(treeCP, q, radioConsulta);
            auto endConsulta = std::chrono::high_resolution_clock::now(); // Detener medición del tiempo de búsqueda
            accesosCP.push_back(accesos);
            puntosEncontradosCP.push_back(puntosEncontrados);
            std::chrono::duration<double> tiempoConsulta = endConsulta - startConsulta;
            tiemposConsultaCP.push_back(tiempoConsulta.count());
        }
        double tiempoTotalConsultasCP = std::accumulate(tiemposConsultaCP.begin(), tiemposConsultaCP.end(), 0.0);
        outfile << "Tiempo total de consultas CP para n=" << n << ": " << tiempoTotalConsultasCP << " segundos" << std::endl;
        outfile << "Tiempo promedio por consulta CP para n=" << n << ": " << tiempoTotalConsultasCP / tiemposConsultaCP.size() << " segundos" << std::endl;

        // Calcular estadísticas para CP:
        // Accesos
        double mediaCP = std::accumulate(accesosCP.begin(), accesosCP.end(), 0.0) / accesosCP.size();
        double varianzaCP = std::accumulate(accesosCP.begin(), accesosCP.end(), 0.0, [mediaCP](double sum, int x) {
            return sum + (x - mediaCP) * (x - mediaCP);
        }) / accesosCP.size();
        double desviacionEstandarCP = std::sqrt(varianzaCP);

        // Puntos
        double mediaPuntosEncontradosCP = std::accumulate(puntosEncontradosCP.begin(), puntosEncontradosCP.end(), 0.0) / puntosEncontradosCP.size();
        double varianzaPuntosEncontradosCP = std::accumulate(puntosEncontradosCP.begin(), puntosEncontradosCP.end(), 0.0, [mediaPuntosEncontradosCP](double sum, int x) {
            return sum + (x - mediaPuntosEncontradosCP) * (x - mediaPuntosEncontradosCP);
        }) / puntosEncontradosCP.size();
        double desviacionEstandarPuntosEncontradosCP = std::sqrt(varianzaPuntosEncontradosCP);

        outfile << "CP - Media de accesos para n=" << n << ": " << mediaCP 
                << ", Desviación estándar: " << desviacionEstandarCP 
                << ", Varianza: " << varianzaCP << std::endl;
        outfile << "CP - Media de puntos encontrados para n=" << n << ": " << mediaPuntosEncontradosCP 
                << ", Desviación estándar: " << desviacionEstandarPuntosEncontradosCP 
                << ", Varianza: " << varianzaPuntosEncontradosCP << std::endl;

        // Margen de error y Intervalo de Confianza
        double z = 1.96; // Valor z para un intervalo de confianza del 95%

        double margenErrorCP = z * (desviacionEstandarCP / std::sqrt(accesosCP.size()));
        double limMinCP = mediaCP - margenErrorCP;
        double limMaxCP = mediaCP + margenErrorCP;
        outfile << "CP - Intervalo de confianza para el número de accesos con n=" << n << ": [" << limMinCP << ", " << limMaxCP << "]" << std::endl;

        double margenErrorPuntosCP = z * (desviacionEstandarPuntosEncontradosCP / std::sqrt(puntosEncontradosCP.size()));
        double icMinPuntosCP = mediaPuntosEncontradosCP - margenErrorPuntosCP;
        double icMaxPuntosCP = mediaPuntosEncontradosCP + margenErrorPuntosCP;
        outfile << "CP - Intervalo de confianza para el número de puntos encontrados con n=" << n << ": [" << icMinPuntosCP << ", " << icMaxPuntosCP << "] \n" << std::endl;
                
        // Imprimir el árbol
        // printTree(rootCP);

        // Limpiar
        delete treeCP.raiz;
    }

    outfile.close();
    return 0;
}