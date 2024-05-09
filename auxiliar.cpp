#include "mtree.hpp"

int altura(Nodo *n) {
	if (n == nullptr) {
		return 0;
	}
	alturaMax = 0;
	for (const auto& entrada : entradas) {
		int h = 0;
		Entrada *actual = entrada;
		while (actual != nullptr) {
			h++;
			actual = actual -> a;
		}
		if (h > alturaMax) {
			alturaMax = h;
		}
	}
	return alturaMax + 1;
}
