# Guía de Trabajo con Git

Si te da flojera usar git en comandos, puedes usar las opciones de vscode relacionadas con git, en las opciones de la derecha, haz click en Source control, normalmente se encuentra abajo de la Lupa, ahí luego de hacer initialize repository, puedes clonar, hacer pull, commit y push.

## 0. La primera vez: Descargar el proyecto (Clonar)
Si es la primera vez que vas a trabajar en el proyecto en tu computadora, debes clonar el repositorio directamente desde GitHub.

Abre tu terminal donde quieras guardar el proyecto y ejecuta:
```bash
git clone https://github.com/FSAguilar/proyectoAlgoritmica.git
```
Luego entra a la carpeta que se acaba de crear:
```bash
cd proyectoAlgoritmica
```
*(¡A partir de aquí, ya no vuelves a usar `clone` nunca más en esta computadora!)*

---
## 1. Antes de empezar a programar: Hacer PULL
**Siempre**, antes de escribir tu primera línea de código en el día, debes descargarte los últimos cambios que hayan subido tus compañeros. Si no haces esto, terminarás trabajando sobre código viejo y causarás problemas.

Abre la terminal en la carpeta del proyecto y ejecuta:
```bash
git pull origin main
```

---

## 2. Al terminar de programar: COMMIT y PUSH
Cuando hayas terminado tu tarea, o si simplemente vas a dejar de programar por hoy, debes subir tus avances para que tus compañeros puedan tenerlos. Son tres pasos:

**A. Preparar todos los archivos modificados:**
```bash
git add .
```
*(El punto `.` le dice a Git que prepare absolutamente todo lo que cambiaste o creaste).*

**B. Guardar el cambio con un mensaje (Commit):**
```bash
git commit -m "Implementada la lectura de grafos"
```
*(Escribe un mensaje claro y corto explicando qué hiciste. No pongas solo "cambios").*

**C. Subir a GitHub (Push):**
```bash
git push origin main
```

---

## 3. ¿Qué es un MERGE y cuándo se usa?

**Merge** significa literalmente "fusionar". En Git, es el proceso de unir dos versiones de código diferentes en una sola.

**¿Cuándo sucede un Merge?**

1. **Cuando haces `git pull` y tienes cambios locales:** 
   Si tú estabas programando y al mismo tiempo un compañero subió cambios a GitHub, cuando hagas `git pull` Git intentará unir automáticamente el código de tu compañero con el tuyo. Esto es un "Merge".
   
2. **Cuando hay "Conflictos" (Merge Conflict):**
   Si tú y tu compañero modificaron **la misma línea del mismo archivo**, Git no es lo suficientemente inteligente para saber quién tiene la razón. Pausará el Merge y te avisará de un "Conflicto". Tendrás que abrir el archivo (VSCode te lo mostrará de colores), elegir qué código conservar (o combinar ambos manualmente), guardar, y hacer un nuevo `git commit` para sellar la fusión.

3. **Cuando se usan Ramas (Branches):**
   Si crearon una rama paralela (ej. `rama-algoritmo-a`) para no romper el programa principal, una vez que esa rama funciona bien, se debe "traer" ese código a la rama principal. Eso se hace ejecutando un comando de fusión explícito (ej. `git merge rama-algoritmo-a`), lo cual combina todo el trabajo experimental con el código estable.

