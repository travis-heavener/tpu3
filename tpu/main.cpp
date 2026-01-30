#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "memory.hpp"
#include "tpu.hpp"

#define MAX_MEMORY_ALLOC 1024 * 1024 * 4 // 4 MiB

#define CERR std::cerr << "Error:\n  "

using namespace tpu;

// Loads a TPU binary image from a file to memory
void loadImageToMemory(tpu::Memory& memory, std::ifstream& handle) {
    // Read entire file to memory (guaranteed to be smaller than memory)
    u32 memPtr = 0;
    const u32 stepSize = 1024;
    do {
        handle.read( reinterpret_cast<char*>(memory.data() + memPtr), stepSize );
        memPtr += stepSize;
    } while (handle.gcount() > 0);
}

int main(int argc, char* argv[]) {
    // Verify args
    if (argc < 2) {
        CERR << "Usage: <tpu> /path/to/image.tpu" << std::endl;
        return EXIT_FAILURE;
    }

    // Verify file exists
    if (!std::filesystem::exists(argv[1]) || !std::filesystem::is_regular_file(argv[1])) {
        CERR << "Invalid TPU image path: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    if (std::filesystem::file_size(argv[1]) > MAX_MEMORY_ALLOC) {
        CERR << "TPU image exceeds memory: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream handle( argv[1], std::ios::binary );
    if (!handle.is_open()) {
        CERR << "Failed to open TPU image: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    // Load TPU image
    std::cout << "Loading memory bank of size " << MAX_MEMORY_ALLOC << " bytes" << std::endl;
    tpu::Memory memory( MAX_MEMORY_ALLOC );

    try {
        loadImageToMemory(memory, handle);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    // Close the file
    handle.close();

    // Initialize the TPU itself
    tpu::TPU tpu;

    try {
        // Start the clock
        tpu.start(memory);
    } catch (tpu::Exception& e) {
        std::cerr << e.what() << std::endl;

        // Dump registers
        tpu.dumpRegs();
    }

    // Dump registers
    tpu.dumpRegs();

    return EXIT_SUCCESS;
}
