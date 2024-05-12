# Tarea 1: M-Tree

Este proyecto implementa dos métodos para construir un árbol métrico (M-Tree) en C. Un M-Tree es un árbol balanceado utilizado para indexar datos basados en métricas, permitiendo búsquedas eficientes. Los dos métodos implementados son el método Ciaccia-Patella (CP) y el método Sexton-Swinbank (SS). El lenguaje de programación a utilizar fue C.

# Estrucutra del Repositorio
```
logaritmos-tarea1/
│
├── ciaccia-patella/         # Código Ciaccia-Patella 
│   ├── output/
│   ├── ciaccia-patella.hpp
│   ├── experiment_cp.cpp 
│   └── mtree_cp.hpp
│
├── sexton-swinbank/         # Código Sexton-Swinbank
│   ├── output/
│   ├── experiment_ss.cpp 
│   ├── mtree_ss.hpp 
│   └── sexton-swinbank.hpp
│
├── README.md                
└── tarea1logaritmos.pdf     # Enunciado
```

# Requisitos
Para compilar y ejecutar este proyecto, se necesita un compilador de C++ que soporte C++11 o superior. No se requieren bibliotecas externas adicionales.

# Compilación y Ejecución
Se recomienda utilizar un entorno de desarrollo integrado (IDE) como Visual Studio Code, CLion, o cualquier otro IDE que soporte C++, para compilar y ejecutar este proyecto. 

Una vez abierto el proyecto en el IDE, navegue al método deseado y ejecute el archivo experiement respectivo, los resultados serán guardados en ciaccia_patella_output.txt y sexton_swinbanck_output.txt.