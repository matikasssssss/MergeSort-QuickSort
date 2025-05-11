#include "mergesort.h"
#include "diskio.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

using namespace std;

/**
 * @brief Intercambia dos elementos dentro de un vector de enteros de 64 bits.
 *
 * @param arr Referencia al vector donde se realizará el intercambio.
 * @param i Índice del primer elemento a intercambiar.
 * @param j Índice del segundo elemento a intercambiar.
 */
void intercambiar_elementos(vector<int64_t> &arr, size_t i, size_t j)
{
    swap(arr[i], arr[j]);
}

/**
 * @brief Compara dos enteros de 64 bits para ordenamiento ascendente.
 *
 * @param a Primer número.
 * @param b Segundo número.
 * @return true si a < b, false en caso contrario.
 */
bool comparar_int64(int64_t a, int64_t b)
{
    return a < b;
}

/**
 * @brief Crea los runs iniciales a partir de un archivo binario no ordenado.
 *
 * Divide el archivo en segmentos de tamaño `a * ELEMENTS_PER_BLOCK`, los ordena en memoria
 * y los escribe en archivos temporales. Estos archivos se usarán luego en la etapa de mezcla.
 *
 * @param entrada Archivo binario de entrada abierto en modo lectura.
 * @param elementos_totales Cantidad total de elementos en el archivo de entrada.
 * @param a Aridad que determina cuántos bloques entran en memoria por run.
 * @param nombres_runs Vector donde se almacenarán los nombres de los archivos de los runs generados.
 *
 * @return Número total de runs creados.
 *
 * @see comparar_int64, leer_bloque, escribir_bloque
 */
size_t crear_runs_iniciales(ifstream &entrada, size_t elementos_totales, size_t a, vector<string> &nombres_runs)
{
    size_t elementos_por_run = a * ELEMENTS_PER_BLOCK;
    size_t num_runs = (elementos_totales + elementos_por_run - 1) / elementos_por_run;
    cout << "Se crearon " << num_runs << " runs iniciales." << endl;

    vector<int64_t> buffer(elementos_por_run);

    for (size_t i = 0; i < num_runs; i++)
    {
        size_t elementos_a_leer = min(elementos_por_run, elementos_totales - i * elementos_por_run);
        size_t bloques_a_leer = (elementos_a_leer + ELEMENTS_PER_BLOCK - 1) / ELEMENTS_PER_BLOCK;
        size_t bloque_inicio = (i * elementos_por_run) / ELEMENTS_PER_BLOCK;

        for (size_t j = 0; j < bloques_a_leer; j++)
        {
            size_t offset = j * ELEMENTS_PER_BLOCK;
            size_t restantes = elementos_a_leer - offset;
            size_t cantidad = min(restantes, (size_t)ELEMENTS_PER_BLOCK);

            leer_bloque(entrada, buffer.data() + offset, bloque_inicio + j, cantidad);
        }

        sort(buffer.begin(), buffer.begin() + elementos_a_leer, comparar_int64);

        string nombre_run = "run_" + to_string(i) + ".bin";
        nombres_runs.push_back(nombre_run);

        ofstream f_run(nombre_run, ios::binary);
        for (size_t j = 0; j < bloques_a_leer; j++)
        {
            size_t offset = j * ELEMENTS_PER_BLOCK;
            size_t restantes = elementos_a_leer - offset;
            size_t cantidad = min(restantes, (size_t)ELEMENTS_PER_BLOCK);

            escribir_bloque(f_run, buffer.data() + offset, j, cantidad);
        }
    }

    return num_runs;
}

/**
 * @brief Estructura que representa un archivo de entrada en la mezcla k-caminos.
 *
 * Contiene un buffer, estado de lectura y control de posición dentro del archivo.
 */
struct RunEntrada
{
    ifstream f;
    int64_t elementos_restantes = 0;
    vector<int64_t> buffer;
    size_t pos = 0;
    size_t elementos_validos = 0;
    size_t bloque_actual = 0;
    bool activo = true;
};

/**
 * @brief Mezcla múltiples archivos ordenados en uno solo usando el algoritmo k-way merge.
 *
 * Lee un bloque de cada archivo de entrada, compara el menor elemento disponible y lo agrega
 * al buffer de salida. Cuando el buffer de salida se llena, se escribe al archivo final.
 *
 * @param input_names Vector de nombres de archivos ordenados a mezclar.
 * @param output_name Nombre del archivo de salida resultante de la mezcla.
 *
 * @see RunEntrada, leer_bloque, escribir_bloque
 */
void mezclar_archivos(const vector<string> &input_names, const string &output_name)
{
    size_t count = input_names.size();
    vector<RunEntrada> entradas(count);
    size_t tam_bloque = ELEMENTS_PER_BLOCK;

    for (size_t i = 0; i < count; i++)
    {
        entradas[i].f.open(input_names[i], ios::binary);
        entradas[i].f.seekg(0, ios::end);
        size_t tam_archivo = entradas[i].f.tellg();
        entradas[i].f.seekg(0);

        entradas[i].elementos_restantes = tam_archivo / sizeof(int64_t);
        entradas[i].buffer.resize(tam_bloque);

        size_t cantidad = min((int64_t)tam_bloque, entradas[i].elementos_restantes);
        cout << "[mezcla] leyendo bloque de run " << i << ", bloque " << entradas[i].bloque_actual << ", cantidad: " << cantidad << endl;
        leer_bloque(entradas[i].f, entradas[i].buffer.data(), entradas[i].bloque_actual, cantidad);
        entradas[i].elementos_validos = cantidad;
        entradas[i].elementos_restantes -= cantidad;
        entradas[i].bloque_actual++;
    }

    ofstream out(output_name, ios::binary);
    vector<int64_t> out_buffer(tam_bloque);
    size_t out_pos = 0;
    size_t bloques_escritos = 0;

    while (true)
    {
        int min_idx = -1;
        for (size_t i = 0; i < count; ++i)
        {
            if (entradas[i].activo && entradas[i].pos < entradas[i].elementos_validos)
            {
                if (min_idx == -1 || entradas[i].buffer[entradas[i].pos] < entradas[min_idx].buffer[entradas[min_idx].pos])
                {
                    min_idx = i;
                }
            }
        }

        if (min_idx == -1)
            break;

        out_buffer[out_pos++] = entradas[min_idx].buffer[entradas[min_idx].pos++];

        if (out_pos == tam_bloque)
        {
            cout << "[mezcla] escribiendo bloque " << bloques_escritos << ", cantidad: " << out_pos << endl;
            escribir_bloque(out, out_buffer.data(), bloques_escritos++, out_pos);
            out_pos = 0;
        }

        if (entradas[min_idx].pos == entradas[min_idx].elementos_validos)
        {
            if (entradas[min_idx].elementos_restantes > 0)
            {
                size_t cantidad = min((int64_t)tam_bloque, entradas[min_idx].elementos_restantes);
                leer_bloque(entradas[min_idx].f, entradas[min_idx].buffer.data(), entradas[min_idx].bloque_actual, cantidad);
                entradas[min_idx].elementos_validos = cantidad;
                entradas[min_idx].elementos_restantes -= cantidad;
                entradas[min_idx].pos = 0;
                entradas[min_idx].bloque_actual++;
            }
            else
            {
                entradas[min_idx].activo = false;
            }
        }
    }

    if (out_pos > 0)
    {
        cout << "[mezcla] escribiendo bloque final (parcial), cantidad: " << out_pos << endl;
        escribir_bloque(out, out_buffer.data(), bloques_escritos++, out_pos);
    }
}

/**
 * @brief Aplica la mezcla jerárquica de runs hasta obtener un solo archivo ordenado.
 *
 * Agrupa los runs existentes de `a` en `a`, los mezcla en archivos intermedios, y repite
 * el proceso recursivamente hasta quedar con un único run. Los archivos temporales anteriores
 * se eliminan tras cada mezcla.
 *
 * @param nombres_runs Vector con los nombres de los archivos de runs actuales.
 * @param a Aridad de la mezcla (número máximo de archivos a mezclar simultáneamente).
 * @param nivel Nivel actual de la recursión (para nombrar los archivos intermedios).
 *
 * @see mezclar_archivos
 */
void mezclar_runs(vector<string> &nombres_runs, size_t a, int nivel)
{
    if (nombres_runs.size() == 1)
    {
        rename(nombres_runs[0].c_str(), "entrada_tmp.bin");
        return;
    }

    size_t nuevos_runs = (nombres_runs.size() + a - 1) / a;
    vector<string> nuevos_nombres;

    for (size_t i = 0; i < nuevos_runs; i++)
    {
        size_t inicio = i * a;
        size_t fin = min(inicio + a, nombres_runs.size());

        string nombre_out = "run_nivel" + to_string(nivel) + "_" + to_string(i) + ".bin";
        nuevos_nombres.push_back(nombre_out);

        vector<string> subset(nombres_runs.begin() + inicio, nombres_runs.begin() + fin);
        mezclar_archivos(subset, nombre_out);

        for (size_t j = inicio; j < fin; j++)
        {
            remove(nombres_runs[j].c_str());
        }
    }

    mezclar_runs(nuevos_nombres, a, nivel + 1);
}

/**
 * @brief Ejecuta el algoritmo de Mergesort Externo sobre un archivo binario.
 *
 * Abre un archivo binario de enteros de 64 bits, crea runs iniciales ordenados,
 * y luego aplica mezclas sucesivas hasta obtener un archivo completamente ordenado.
 *
 * @param nombre_archivo Nombre del archivo binario de entrada.
 * @param a Aridad de la mezcla. Define tanto el tamaño de los runs como la cantidad de archivos mezclados simultáneamente.
 *
 * @post El archivo final ordenado se guarda con el nombre "entrada_tmp.bin".
 *
 * @see crear_runs_iniciales, mezclar_runs
 */
void mergesort_externo(const string &nombre_archivo, size_t a)
{
    ifstream entrada(nombre_archivo, ios::binary);
    if (!entrada)
    {
        perror("fopen");
        exit(1);
    }

    entrada.seekg(0, ios::end);
    size_t tam_archivo = entrada.tellg();
    size_t elementos_totales = tam_archivo / sizeof(int64_t);
    entrada.seekg(0);

    vector<string> nombres_runs;
    crear_runs_iniciales(entrada, elementos_totales, a, nombres_runs);
    entrada.close();

    mezclar_runs(nombres_runs, a, 0);
}
