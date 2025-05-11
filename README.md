# Tarea 1: Mergesort Externo vs Quicksort Externo

**Curso**: CC4102 - Diseño y Análisis de Algoritmos  
**Universidad**: DCC - Universidad de Chile  
**Profesores**: Benjamín Bustos, Gonzalo Navarro  
**Auxiliares**: Sergio Rojas, Pablo Skewes

## Descripción

Este proyecto implementa y compara dos algoritmos de ordenamiento para memoria externa:

- **Mergesort Externo**, con aridad configurable.
- **Quicksort Externo**, basado en selección de pivotes desde disco.

El objetivo es medir el desempeño de ambos algoritmos en términos de tiempo de ejecución y accesos a disco (lecturas y escrituras), siguiendo las metodologías señaladas en la tarea.

## Estructura del Proyecto

```
.
├── diskio.h/.cpp          # Funciones de acceso a disco por bloque (lectura y escritura)
├── mergesort.h/.cpp       # Implementación de Mergesort externo
├── quicksort.h/.cpp       # Implementación de Quicksort externo
├── utils.h/.cpp           # Utilidades: generación de archivos binarios
├── buscar_a_optimo.h/.cpp # Búsqueda de aridad óptima para mergesort
├── main_buscar_a.cpp      # Ejecuta la búsqueda de aridad óptima (a)
├── main_experimentos.cpp  # Ejecuta los experimentos comparativos entre ambos algoritmos
├── resultados.txt         # Archivo generado con los resultados de los experimentos
```

## Requisitos

- Compilador C++17 o superior.
- Docker (opcional pero recomendado para validación bajo límites de memoria).
- No se utilizan librerías externas. Solo `C++ STL`.

## Compilación

Para compilar el proyecto:

```bash
g++ -std=c++17 -O2 -o buscar_a main_buscar_a.cpp mergesort.cpp diskio.cpp utils.cpp buscar_a_optimo.cpp

g++ -std=c++17 -O2 -o experimento main_experimentos.cpp mergesort.cpp quicksort.cpp diskio.cpp utils.cpp
```

## ▶️ Ejecución

### 1. Buscar aridad óptima `a` para Mergesort Externo:

```bash
./buscar_a
```

Este comando:

- Genera un archivo `entrada.bin` de 3000MB con números aleatorios.
- Ejecuta Mergesort externo con diferentes valores de `a`.
- Imprime el valor óptimo (usando búsqueda ternaria) que minimiza el número de I/Os.

### 2. Ejecutar experimentos comparativos:

```bash
./experimento
```

Este comando:

- Genera 5 archivos aleatorios de 𝑁, con 𝑁 ∈ {4𝑀, 8𝑀, …60𝑀}.
- Aplica Mergesort y Quicksort Externo a cada archivo (usando la misma aridad `a = 60`).
- Mide tiempo, lecturas y escrituras.
- Verifica si el resultado está ordenado.
- Registra todo en consola y en el archivo `resultados.txt`.

## Resultados

Los resultados incluyen:

- Tiempo de ejecución para cada algoritmo.
- Número total de lecturas y escrituras a disco.
- Verificación de orden final.
- Están disponibles en `resultados.txt`.

## Ejecución en Docker (opcional)

Para correr en entorno controlado (memoria limitada a 50MB):

```bash
docker pull pabloskewes/cc4102-cpp-env
docker run --rm -it -m 50m -v "$PWD":/workspace pabloskewes/cc4102-cpp-env bash
```

Dentro del contenedor:

```bash
cd ../MergeSort-QuickSort # ubicación de la carpeta con el experimento
make
./buscar_a
./experimento
```

## Limpieza

Para eliminar archivos temporales:

```bash
rm *.bin resultados.txt buscar_a experimento
```

## Créditos

Implementado por Roberto Vega, Gaspar Robledo y Matías Farías, ambientado en la Tarea 1 del curso CC4102.  
Todos los algoritmos fueron desarrollados desde cero, sin el uso de librerías externas de ordenamiento para su correcto análisis y experimentación.

---
