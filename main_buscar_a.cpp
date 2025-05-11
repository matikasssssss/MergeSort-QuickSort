#include <iostream>
#include <fstream>
#include "utils.h"
#include "diskio.h"
#include "mergesort.h"
#include "buscar_a_optimo.h"

#define ARCHIVO_PRUEBA "entrada.bin"

using namespace std;

/**
 * @brief Punto de entrada para determinar la aridad óptima (a) del algoritmo Mergesort Externo.
 *
 * Este programa realiza lo siguiente:
 * 1. Genera un archivo binario de prueba llamado `"entrada.bin"` con números `int64_t` aleatorios de tamaño `size_in_mb`.
 * 2. Llama a la función `buscar_a_optimo()` para encontrar el valor de aridad `a` que minimiza los accesos a disco
 *    al ejecutar Mergesort Externo sobre el archivo generado.
 * 3. Imprime la mejor aridad encontrada por consola.
 *
 * Parámetros predefinidos:
 * - Tamaño del archivo de prueba: 50 MB
 * - Tamaño de bloque (`BLOCK_SIZE`) definido en `diskio.h` (usualmente 4096 bytes)
 *
 * @return 0 al finalizar correctamente.
 *
 * @see generate_binary_file, buscar_a_optimo, mergesort_externo
 */
int main()
{
    const size_t size_in_mb = 60 * 50;
    cout << " Generando archivo binario de prueba (" << size_in_mb << " MB)...\n";
    generate_binary_file(ARCHIVO_PRUEBA, size_in_mb);

    cout << " Buscando aridad optima (a) mediante busqueda ternaria...\n";
    size_t a_optimo = buscar_a_optimo(ARCHIVO_PRUEBA, BLOCK_SIZE);

    cout << " Aridad optima encontrada: a = " << a_optimo << endl;

    return 0;
}
