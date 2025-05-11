#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cstdio>
#include "utils.h"
#include "quicksort.h"
#include "mergesort.h"
#include "diskio.h"

using namespace std;

/**
 * @brief Verifica que los elementos en un archivo binario estén ordenados en orden no decreciente.
 *
 * Recorre el archivo binario leído por bloques de tamaño `sizeof(int64_t)` y comprueba que
 * cada valor sea mayor o igual que el anterior. Si encuentra un par fuera de orden, retorna false.
 *
 * @param filename Nombre del archivo binario a verificar.
 * @return true si el archivo está ordenado, false si hay algún par de elementos fuera de orden.
 */
bool verificar_orden(const string &filename)
{
    ifstream entrada(filename, ios::binary);
    if (!entrada)
    {
        cerr << "Error al abrir el archivo para verificación.\n";
        return false;
    }

    int64_t prev, curr;
    if (!entrada.read(reinterpret_cast<char *>(&prev), sizeof(int64_t)))
        return true;

    while (entrada.read(reinterpret_cast<char *>(&curr), sizeof(int64_t)))
    {
        if (prev > curr)
            return false;
        prev = curr;
    }

    return true;
}

/**
 * @brief Copia el contenido de un archivo binario a otro.
 *
 * Se utiliza para duplicar el archivo original en dos versiones separadas:
 * una para Mergesort y otra para Quicksort, manteniendo los mismos datos para comparar rendimientos.
 *
 * @param origen Nombre del archivo de entrada (original).
 * @param destino Nombre del archivo de destino.
 */
void copiar_archivo(const string &origen, const string &destino)
{
    ifstream src(origen, ios::binary);
    ofstream dst(destino, ios::binary);
    dst << src.rdbuf();
}

/**
 * @brief Función principal que ejecuta la comparación entre Mergesort Externo y Quicksort Externo.
 *
 * Para cada uno de los 5 archivos binarios generados aleatoriamente de tamaño `size_in_mb`, se realiza:
 * 1. Generación del archivo binario desordenado.
 * 2. Duplicado del archivo para ambos algoritmos.
 * 3. Ejecución de Mergesort externo y medición de tiempo, lecturas y escrituras.
 * 4. Ejecución de Quicksort externo y medición de tiempo, lecturas y escrituras.
 * 5. Verificación de que los resultados estén ordenados.
 * 6. Registro de resultados en consola y archivo `resultados.txt`.
 *
 * Parámetros usados:
 * - M = 50 MB (memoria disponible)
 * - a = 60 (aridad tanto para mezcla como para partición)
 * - 5 archivos de entrada de tamaño 4M (4×50 MB = 200 MB en total)
 *
 * @return 0 si todo se ejecuta correctamente.
 *
 * @note Usa contadores globales `cont_lecturas` y `cont_escrituras` para obtener estadísticas de acceso a disco.
 * @see mergesort_externo, quicksort_externo, verificar_orden, generate_binary_file
 */
int main()
{
    const size_t archivos = 5;
    const size_t size_in_mb = 4 * 50;
    const size_t M = 50 * 1024 * 1024;
    const size_t a = 60;

    ofstream resultados("resultados.txt");

    for (size_t i = 0; i < archivos; ++i)
    {
        string original = "original_" + to_string(i) + ".bin";
        string merge_in = "merge_input_" + to_string(i) + ".bin";
        string quick_in = "quick_input_" + to_string(i) + ".bin";

        cout << "\n==================  ARCHIVO #" << i << " ==================\n";
        resultados << "\n==================  ARCHIVO #" << i << " ==================\n";

        cout << " Generando archivo binario original...\n";
        resultados << " Generando archivo binario original...\n";
        generate_binary_file(original, size_in_mb);

        cout << " Copiando archivo para Mergesort y Quicksort...\n";
        resultados << " Copiando archivo para Mergesort y Quicksort...\n";
        copiar_archivo(original, merge_in);
        copiar_archivo(original, quick_in);

        // MERGESORT
        cout << "\n Ejecutando Mergesort Externo...\n";
        resultados << "\n Ejecutando Mergesort Externo...\n";
        cont_lecturas = 0;
        cont_escrituras = 0;
        auto inicio_merge = chrono::high_resolution_clock::now();
        mergesort_externo(merge_in, a);
        auto fin_merge = chrono::high_resolution_clock::now();

        double tiempo_merge = chrono::duration<double>(fin_merge - inicio_merge).count();
        size_t lecturas_merge = cont_lecturas;
        size_t escrituras_merge = cont_escrituras;

        cout << " Verificando orden Mergesort...\n";
        resultados << " Verificando orden Mergesort...\n";
        bool orden_merge = verificar_orden("entrada_tmp.bin");

        remove("entrada_tmp.bin");
        remove(merge_in.c_str());

        // QUICKSORT
        cout << "\n Ejecutando Quicksort Externo...\n";
        resultados << "\n Ejecutando Quicksort Externo...\n";
        cont_lecturas = 0;
        cont_escrituras = 0;
        auto inicio_quick = chrono::high_resolution_clock::now();
        quicksort_externo(quick_in, M, a, 0);
        auto fin_quick = chrono::high_resolution_clock::now();

        double tiempo_quick = chrono::duration<double>(fin_quick - inicio_quick).count();
        size_t lecturas_quick = cont_lecturas;
        size_t escrituras_quick = cont_escrituras;

        cout << " Verificando orden Quicksort...\n";
        resultados << " Verificando orden Quicksort...\n";
        bool orden_quick = verificar_orden(quick_in);

        remove(quick_in.c_str());
        remove(original.c_str());

        // RESULTADOS
        cout << "\n RESULTADOS PARA ARCHIVO #" << i << "\n";
        resultados << "\n RESULTADOS PARA ARCHIVO #" << i << "\n";

        cout << "Archivo generado: " << size_in_mb << " MB (" << size_in_mb * 1024 * 1024 / sizeof(int64_t) << " elementos)\n";
        resultados << "Archivo generado: " << size_in_mb << " MB (" << size_in_mb * 1024 * 1024 / sizeof(int64_t) << " elementos)\n";

        cout << "Memoria M: " << M / (1024 * 1024) << " MB\n";
        resultados << "Memoria M: " << M / (1024 * 1024) << " MB\n";

        cout << "Aridad a: " << a << "\n\n";
        resultados << "Aridad a: " << a << "\n\n";

        cout << "MERGESORT EXTERNO\n";
        resultados << "MERGESORT EXTERNO\n";
        cout << "Tiempo: " << tiempo_merge << " segundos\n";
        resultados << "Tiempo: " << tiempo_merge << " segundos\n";
        cout << "Lecturas: " << lecturas_merge << "\n";
        resultados << "Lecturas: " << lecturas_merge << "\n";
        cout << "Escrituras: " << escrituras_merge << "\n";
        resultados << "Escrituras: " << escrituras_merge << "\n";
        cout << "Orden correcto: " << (orden_merge ? "Sí" : "No") << "\n\n";
        resultados << "Orden correcto: " << (orden_merge ? "Sí" : "No") << "\n\n";

        cout << "QUICKSORT EXTERNO\n";
        resultados << "QUICKSORT EXTERNO\n";
        cout << "Tiempo: " << tiempo_quick << " segundos\n";
        resultados << "Tiempo: " << tiempo_quick << " segundos\n";
        cout << "Lecturas: " << lecturas_quick << "\n";
        resultados << "Lecturas: " << lecturas_quick << "\n";
        cout << "Escrituras: " << escrituras_quick << "\n";
        resultados << "Escrituras: " << escrituras_quick << "\n";
        cout << "Orden correcto: " << (orden_quick ? "Sí" : "No") << "\n";
        resultados << "Orden correcto: " << (orden_quick ? "Sí" : "No") << "\n";
    }

    return 0;
}