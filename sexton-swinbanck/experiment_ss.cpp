#include "sexton-swinbanck.hpp"

// Función para realizar las consultas
std::pair<int, int> query(const MTree& tree, Punto q, double radio) {
    int accesos = 0; // Contador de accesos
    int puntosEncontrados = 0; // Contador de puntos encontrados

    // Caso base
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


// Experimento para n = 2^10, 2^11, ..., 2^25
int main() {
    std::ofstream outfile("sexton_swinbanck_output.txt");

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

        // Construir el árbol con SS
        auto startSS = std::chrono::high_resolution_clock::now(); // Comenzar a medir el tiempo de construcción
        Nodo* rootSS = SS(P, b, B); // Obtengo el Nodo raíz
        MTree treeSS(rootSS); // Creo su árbol con esta raíz
        auto endSS = std::chrono::high_resolution_clock::now(); // Detener medición del tiempo de construcción
        std::chrono::duration<double> tiempoConstruccionSS = endSS - startSS;
        outfile << "Tiempo de construcción SS para n=" << n << ": " << tiempoConstruccionSS.count() << " segundos" << std::endl;

        // Evaluar consultas
        std::vector<int> accesosSS;
        std::vector<int> puntosEncontradosSS;
        std::vector<double> tiemposConsultaSS;

        for (const auto& q : Q) {
            auto startConsulta = std::chrono::high_resolution_clock::now(); // Comenzar a medir el tiempo de búsqueda
            auto [accesos, puntosEncontrados] = query(treeSS, q, radioConsulta);
            auto endConsulta = std::chrono::high_resolution_clock::now(); // Detener medición del tiempo de búsqueda
            accesosSS.push_back(accesos);
            puntosEncontradosSS.push_back(puntosEncontrados);
            std::chrono::duration<double> tiempoConsulta = endConsulta - startConsulta;
            tiemposConsultaSS.push_back(tiempoConsulta.count());
        }
        double tiempoTotalConsultasSS = std::accumulate(tiemposConsultaSS.begin(), tiemposConsultaSS.end(), 0.0);
        outfile << "Tiempo total de consultas SS para n=" << n << ": " << tiempoTotalConsultasSS << " segundos" << std::endl;
        outfile << "Tiempo promedio por consulta SS para n=" << n << ": " << tiempoTotalConsultasSS / tiemposConsultaSS.size() << " segundos" << std::endl;

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

        outfile << "SS - Media de accesos para n=" << n << ": " << mediaSS 
                << ", Desviación estándar: " << desviacionEstandarSS 
                << ", Varianza: " << varianzaSS << std::endl;
        outfile << "SS - Media de puntos encontrados para n=" << n << ": " << mediaPuntosEncontradosSS 
                << ", Desviación estándar: " << desviacionEstandarPuntosEncontradosSS 
                << ", Varianza: " << varianzaPuntosEncontradosSS << std::endl;

        // Margen de error y Intervalo de Confianza
        double z = 1.96; // Valor z para un intervalo de confianza del 95%

        double margenErrorSS = z * (desviacionEstandarSS / std::sqrt(accesosSS.size()));
        double limMinSS = mediaSS - margenErrorSS;
        double limMaxSS = mediaSS + margenErrorSS;
        outfile << "SS - Intervalo de confianza para el número de accesos con n=" << n << ": [" << limMinSS << ", " << limMaxSS << "]" << std::endl;

        double margenErrorPuntosSS = z * (desviacionEstandarPuntosEncontradosSS / std::sqrt(puntosEncontradosSS.size()));
        double icMinPuntosSS = mediaPuntosEncontradosSS - margenErrorPuntosSS;
        double icMaxPuntosSS = mediaPuntosEncontradosSS + margenErrorPuntosSS;
        outfile << "SS - Intervalo de confianza para el número de puntos encontrados con n=" << n << ": [" << icMinPuntosSS << ", " << icMaxPuntosSS << "] \n" << std::endl;

        // Limpiar
        delete rootSS;
    }

    outfile.close();
    return 0;
}
