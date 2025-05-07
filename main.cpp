#include <iostream>
#include <string>
#include "utils.h"     // Contiene generate_binary_file
#include "quicksort.h" // Debe usar leer_bloque y escribir_bloque internamente
#include "diskio.h"    // Contiene cont_lecturas y cont_escrituras

using namespace std;

// Verificar si el archivo está ordenado
bool verificar_orden(const string &filename)
{
    ifstream entrada(filename, ios::binary);
    if (!entrada)
    {
        cerr << "Error al abrir el archivo para verificación.\n";
        return false;
    }

    int64_t prev, curr;
    entrada.read(reinterpret_cast<char *>(&prev), sizeof(int64_t));

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

int main()
{
    const string filename = "datos.bin";
    const size_t size_in_mb = 8 * 50; // Tamaño del archivo en MB
    const size_t M = 8 * 1024 * 1024; // Memoria máxima en bytes (ej: 8 MB)
    const size_t a = 172;             // Número de particiones (pivotes)

    cout << "Generando archivo binario...\n";
    generate_binary_file(filename, size_in_mb);

    cout << "Ordenando archivo...\n";
    quicksort_externo(filename, M, a, 0); // Usa internamente leer_bloque y escribir_bloque

    cout << "Verificando orden...\n";
    if (verificar_orden(filename))
    {
        cout << "Archivo ordenado correctamente.\n";
    }
    else
    {
        cout << "Archivo no está ordenado.\n";
    }

    // Mostrar estadísticas
    cout << "\n📊 Estadísticas de E/S:\n";
    cout << "Lecturas realizadas: " << cont_lecturas << '\n';
    cout << "Escrituras realizadas: " << cont_escrituras << '\n';

    return 0;
}