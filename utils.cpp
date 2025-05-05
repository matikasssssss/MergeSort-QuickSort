#include "utils.h"
#include <fstream>
#include <iostream>
#include <random>
#include <cstdint>

using namespace std;

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
