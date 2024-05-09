#include "mtree.hpp"

int altura(Nodo *n) {
	// Si el nodo está vacío no tiene altura
	if (n == nullptr) {
		return 0;
	}
	alturaMax = 0;
	// Se revisan las alturas de cada entrada dentro de las entradas del nodo
	for (const auto& entrada : entradas) {
		int h = 0;
		Entrada *actual = entrada;
		// Se revisa cada hijo de la entrada, por cada uno se suma 1 a la altura
		while (actual != nullptr) {
			h++;
			actual = actual -> a;
		}
		// Se elige la mayor cadena de entradas posible
		if (h > alturaMax) {
			alturaMax = h;
		}
	}
	// Se devuelve la cadena más larga encontrada +1 (el propio nodo)
	return alturaMax + 1;
}
