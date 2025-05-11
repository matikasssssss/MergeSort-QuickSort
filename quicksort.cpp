#include "diskio.h"
#include "quicksort.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <cstdio>

using namespace std;

/**
 * @brief Ejecuta el algoritmo de Quicksort Externo sobre un archivo binario de enteros de 64 bits.
 *
 * Quicksort externo permite ordenar archivos que no caben completamente en memoria principal.
 * La función realiza una partición del archivo original en `a` subarchivos usando `a-1` pivotes seleccionados
 * aleatoriamente desde un bloque, y luego ordena cada subarchivo recursivamente hasta que puedan
 * ser ordenados en memoria. Finalmente, concatena las particiones ya ordenadas en el archivo original.
 *
 * Etapas del algoritmo:
 * 1. Si el archivo cabe en memoria (`N ≤ M`), se ordena directamente en RAM.
 * 2. Si no cabe:
 *    - Se selecciona un bloque aleatorio del archivo y se eligen `a-1` pivotes.
 *    - Se particiona el archivo en `a` subarchivos en base a los pivotes.
 *    - Cada subarchivo se ordena recursivamente con Quicksort Externo.
 *    - Las particiones ordenadas se concatenan en el archivo original.
 *
 * @param archivo Nombre del archivo binario a ordenar (tipo `int64_t`, formato binario).
 * @param M Tamaño máximo de memoria principal permitida en bytes.
 * @param a Número de particiones en cada división (cantidad de subarreglos o aridad del algoritmo).
 * @param nivel Nivel de recursión actual. Se usa para nombrar los archivos temporales generados.
 *
 * @pre El archivo debe existir, estar en formato binario y contener únicamente elementos `int64_t`.
 *
 * @post El archivo se sobrescribe con su versión ordenada. Los archivos temporales generados son eliminados.
 *
 * @note Utiliza acceso a disco por bloques (usando `leer_bloque` y `escribir_bloque`) de tamaño `ELEMENTS_PER_BLOCK`.
 *       También utiliza un contador global `cont_escrituras` para llevar la cuenta de escrituras a disco.
 *
 * @see leer_bloque, escribir_bloque
 */
void quicksort_externo(const string &archivo, size_t M, size_t a, int nivel)
{
    ifstream entrada(archivo, ios::binary);
    if (!entrada)
    {
        perror("fopen");
        exit(1);
    }

    entrada.seekg(0, ios::end);
    size_t tam_archivo = entrada.tellg();
    size_t total_elementos = tam_archivo / sizeof(int64_t);
    entrada.seekg(0);

    size_t max_en_memoria = M / sizeof(int64_t);

    if (total_elementos <= max_en_memoria)
    {
        vector<int64_t> buffer(total_elementos);
        entrada.read(reinterpret_cast<char *>(buffer.data()), tam_archivo);
        if (!entrada)
        {
            cerr << "Error leyendo el archivo.\n";
            exit(1);
        }

        sort(buffer.begin(), buffer.end());
        entrada.close();

        ofstream salida(archivo, ios::binary);
        salida.write(reinterpret_cast<const char *>(buffer.data()), tam_archivo);
        if (!salida)
        {
            cerr << "Error escribiendo el archivo.\n";
            exit(1);
        }
        return;
    }

    // Selección de pivotes
    vector<int64_t> muestra(ELEMENTS_PER_BLOCK);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<size_t> dis(0, total_elementos / ELEMENTS_PER_BLOCK - 1);
    size_t bloque_aleatorio = dis(gen);
    leer_bloque(entrada, muestra.data(), bloque_aleatorio, ELEMENTS_PER_BLOCK);

    shuffle(muestra.begin(), muestra.end(), gen);
    vector<int64_t> pivotes(muestra.begin(), muestra.begin() + a - 1);
    sort(pivotes.begin(), pivotes.end());

    entrada.seekg(0);

    // Archivos de salida por partición
    vector<ofstream> salidas(a);
    vector<string> nombres_partes(a);
    for (size_t i = 0; i < a; ++i)
    {
        nombres_partes[i] = "quick_part_" + to_string(nivel) + "_" + to_string(i) + ".bin";
        salidas[i].open(nombres_partes[i], ios::binary);
        if (!salidas[i])
        {
            cerr << "Error creando archivo de salida " << nombres_partes[i] << "\n";
            exit(1);
        }
    }

    // Buffers por partición con flushing a disco
    vector<vector<int64_t>> buffers_tmp(a);
    for (size_t i = 0; i < a; ++i)
        buffers_tmp[i].reserve(ELEMENTS_PER_BLOCK);

    vector<int64_t> buffer(ELEMENTS_PER_BLOCK);
    size_t bloques = (total_elementos + ELEMENTS_PER_BLOCK - 1) / ELEMENTS_PER_BLOCK;

    for (size_t i = 0; i < bloques; ++i)
    {
        size_t cantidad = min(ELEMENTS_PER_BLOCK, total_elementos - i * ELEMENTS_PER_BLOCK);
        leer_bloque(entrada, buffer.data(), i, cantidad);

        for (size_t j = 0; j < cantidad; ++j)
        {
            int64_t x = buffer[j];
            size_t idx = lower_bound(pivotes.begin(), pivotes.end(), x) - pivotes.begin();
            buffers_tmp[idx].push_back(x);

            if (buffers_tmp[idx].size() == ELEMENTS_PER_BLOCK)
            {
                salidas[idx].write(reinterpret_cast<char *>(buffers_tmp[idx].data()), ELEMENTS_PER_BLOCK * sizeof(int64_t));
                cont_escrituras++;
                buffers_tmp[idx].clear();
            }
        }
    }

    entrada.close();

    // Vaciar buffers restantes
    for (size_t i = 0; i < a; ++i)
    {
        if (!buffers_tmp[i].empty())
        {
            escribir_bloque(salidas[i], buffers_tmp[i].data(), -1, buffers_tmp[i].size());
            cont_escrituras++;
            buffers_tmp[i].clear();
        }
        salidas[i].close();
    }

    // Ordenar recursivamente
    for (size_t i = 0; i < a; ++i)
        quicksort_externo(nombres_partes[i], M, a, nivel + 1);

    // Concatenar particiones
    ofstream salida_final(archivo, ios::binary);
    vector<int64_t> out_buffer(ELEMENTS_PER_BLOCK);

    for (size_t i = 0; i < a; ++i)
    {
        ifstream in(nombres_partes[i], ios::binary);
        in.seekg(0, ios::end);
        size_t tam = in.tellg();
        size_t elementos = tam / sizeof(int64_t);
        in.seekg(0);

        size_t bloques_leer = (elementos + ELEMENTS_PER_BLOCK - 1) / ELEMENTS_PER_BLOCK;
        for (size_t j = 0; j < bloques_leer; ++j)
        {
            size_t cantidad = min(ELEMENTS_PER_BLOCK, elementos - j * ELEMENTS_PER_BLOCK);
            leer_bloque(in, out_buffer.data(), j, cantidad);
            escribir_bloque(salida_final, out_buffer.data(), -1, cantidad);
        }

        in.close();
        remove(nombres_partes[i].c_str());
    }
}
