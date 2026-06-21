AQUI VIENE LA IMPLEMENTACION DE LOS HEADERS(.h) en include
Es como implementar una interfaz, es decir se implementan todas las funciones declaradas en el header. Aclarar que no se pueden poner funciones extra, porque de lo que se hace include es el header, no este .cpp

Ejemplo
``` cpp
#include "matematicas.h"
#include <cmath> // Librerias necesarias, con <bits/stdc++.h> nos basta

// Implementar Funciones
int sumar(int a, int b) {
    return a + b;
}

float calcularDistancia(Punto p1, Punto p2) {
    int distX = p2.x - p1.x;
    int distY = p2.y - p1.y;
    return std::sqrt(distX * distX + distY * distY);
}

```

A parte el main.cpp,
también podemos separar los "módulos" en carpetas, por ejemplo una carpeta Djikstra, otra Flood-Fill, etc.