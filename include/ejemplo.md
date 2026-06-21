AQUI VIENEN LOS ARCHIVOS .h
DONDE SE DECLARAN DATOS Y FUNCIONES PARA HACER INCLUDE
SON COMO CLASES (No es necesario crear una clase, puedes tener solo funciones, structs o datos sueltos)

Ejemplo de .h
``` cpp
#ifndef MATEMATICAS_H //Para no importar por accidente 2 veces
#define MATEMATICAS_H


const float PI = 3.14159f;


struct Punto {
    int x;
    int y;
};


int sumar(int a, int b); // funciones abstractas
float calcularDistancia(Punto p1, Punto p2);


#endif

```