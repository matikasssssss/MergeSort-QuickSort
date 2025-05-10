#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cstdio>
#include "utils.h"     // generate_binary_file
#include "quicksort.h" // quicksort_externo
#include "mergesort.h" // mergesort_externo
#include "diskio.h"    // cont_lecturas, cont_escrituras

using namespace std;

// Verifica si un archivo binario está ordenado
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
        return true; // Archivo vacío o con 1 elemento

    while (entrada.read(reinterpret_cast<char *>(&curr), sizeof(int64_t)))
    {
        if (prev > curr)
        {
            cerr << "Archivo no está ordenado.\n";
            return false;
        }
        prev = curr;
    }

    return true;
}

// Copia binaria entre archivos
void copiar_archivo(const string &origen, const string &destino)
{
    ifstream src(origen, ios::binary);
    ofstream dst(destino, ios::binary);
    dst << src.rdbuf();
}

int main()
{
    const string archivo_original = "original.bin";
    const string archivo_merge = "merge_input.bin";
    const string archivo_quick = "quick_input.bin";

    const size_t size_in_mb = 10 * 50;
    const size_t M = 50 * 1024 * 1024; // Memoria disponible en bytes (8 MB)
    const size_t a = 342;              // Aridad / cantidad de particiones

    cout << " Generando archivo binario original...\n";
    generate_binary_file(archivo_original, size_in_mb);

    cout << " Copiando archivo para Mergesort y Quicksort...\n";
    copiar_archivo(archivo_original, archivo_merge);
    copiar_archivo(archivo_original, archivo_quick);

    // ======================== MERGESORT ========================
    cout << "\n Ejecutando Mergesort Externo...\n";
    cont_lecturas = 0;
    cont_escrituras = 0;
    auto inicio_merge = chrono::high_resolution_clock::now();
    mergesort_externo(archivo_merge, a);
    auto fin_merge = chrono::high_resolution_clock::now();

    double tiempo_merge = chrono::duration<double>(fin_merge - inicio_merge).count();
    size_t lecturas_merge = cont_lecturas;
    size_t escrituras_merge = cont_escrituras;

    cout << " Verificando orden Mergesort...\n";
    bool orden_merge = verificar_orden("entrada_tmp.bin"); // output final mergesort

    // Eliminar archivo temporal de salida de mergesort
    remove("entrada_tmp.bin");
    // Eliminar copia de entrada usada por mergesort
    remove(archivo_merge.c_str());

    // ======================== QUICKSORT ========================
    cout << "\n Ejecutando Quicksort Externo...\n";
    cont_lecturas = 0;
    cont_escrituras = 0;
    auto inicio_quick = chrono::high_resolution_clock::now();
    quicksort_externo(archivo_quick, M, a, 0);
    auto fin_quick = chrono::high_resolution_clock::now();

    double tiempo_quick = chrono::duration<double>(fin_quick - inicio_quick).count();
    size_t lecturas_quick = cont_lecturas;
    size_t escrituras_quick = cont_escrituras;

    cout << " Verificando orden Quicksort...\n";
    bool orden_quick = verificar_orden(archivo_quick);

    // Eliminar archivo quick_input.bin
    remove(archivo_quick.c_str());
    remove(archivo_original.c_str());
    // ======================== RESULTADOS ========================
    cout << "\n==================  RESULTADOS ==================\n";
    cout << "Archivo generado: " << size_in_mb << " MB (" << size_in_mb * 1024 * 1024 / sizeof(int64_t) << " elementos)\n";
    cout << "Memoria M: " << M / (1024 * 1024) << " MB\n";
    cout << "Aridad a: " << a << "\n\n";

    cout << "MERGESORT EXTERNO\n";
    cout << "Tiempo: " << tiempo_merge << " segundos\n";
    cout << "Lecturas: " << lecturas_merge << "\n";
    cout << "Escrituras: " << escrituras_merge << "\n";
    cout << "Orden correcto: " << (orden_merge ? "Si" : "No") << "\n\n";

    cout << "QUICKSORT EXTERNO\n";
    cout << "Tiempo: " << tiempo_quick << " segundos\n";
    cout << "Lecturas: " << lecturas_quick << "\n";
    cout << "Escrituras: " << escrituras_quick << "\n";
    cout << "Orden correcto: " << (orden_quick ? "Si" : "No") << "\n";

    return 0;
}
