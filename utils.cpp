#include "utils.h"
#include <fstream>
#include <iostream>
#include <random>
#include <cstdint>

using namespace std;

/**
 * @brief Genera un archivo binario con números enteros aleatorios de 64 bits.
 *
 * Esta función crea un archivo binario de nombre `filename` y tamaño aproximado
 * `size_in_mb` megabytes, escribiendo en él números enteros sin signo de 64 bits (`uint64_t`)
 * generados aleatoriamente mediante una distribución uniforme.
 *
 * La generación utiliza una semilla fija para asegurar la reproducibilidad.
 *
 * @param filename Nombre del archivo binario a crear.
 * @param size_in_mb Tamaño del archivo a generar en megabytes.
 *
 * @note El archivo se escribe completamente en modo binario. Se utiliza `mt19937_64`
 * como generador de números aleatorios y `uniform_int_distribution<uint64_t>` para
 * la distribución uniforme.
 *
 * @warning Si el archivo no puede crearse, se mostrará un mensaje de error por `cerr`.
 */
void generate_binary_file(const string &filename, size_t size_in_mb)
{
    ofstream file(filename, ios::binary);
    if (!file)
    {
        cerr << "Error creating file: " << filename << endl;
        return;
    }

    size_t num_values = (size_in_mb * 1024 * 1024) / sizeof(uint64_t);
    mt19937_64 rng(42); // Semilla fija
    uniform_int_distribution<uint64_t> dist;

    for (size_t i = 0; i < num_values; ++i)
    {
        uint64_t value = dist(rng);
        file.write(reinterpret_cast<const char *>(&value), sizeof(uint64_t));
    }

    file.close();
    cout << "File created: " << filename << " (" << size_in_mb << " MB, "
         << num_values << " values)" << endl;
}
