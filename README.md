# *Tarea 1: M-Tree*

Este proyecto implementa dos métodos para construir un árbol métrico (M-Tree) en C. Un M-Tree es un árbol balanceado utilizado para indexar datos basados en métricas, permitiendo búsquedas eficientes. Los dos métodos implementados son el método Ciaccia-Patella (CP) y el método Sexton-Swinbank (SS). El lenguaje de programación a utilizar fue C.

## *Estructuras de Datos*

Punto: Representa un punto en un plano bidimensional con coordenadas x e y.

NodoMTree: Representa un nodo en el M-Tree, con un punto central p, un radio de cobertura cr, y una lista de hijos.

Cluster: Estructura auxiliar utilizada para agrupar puntos durante el proceso de construcción del M-Tree.

## *Funciones Clave*

distancia: Calcula la distancia euclidiana entre dos puntos.
crearNodo: Crea un nodo para el M-Tree con un punto y un radio de cobertura.

find_closest_clusters: Encuentra los dos clusters más cercanos dentro de un conjunto de clusters.

## *Métodos de Construcción*

Método Ciaccia-Patella (CP): 

Método Sexton-Swinbank (SS): 

## *Instrucciones*