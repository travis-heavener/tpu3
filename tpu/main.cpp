#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "memory.hpp"

#define MAX_MEMORY_ALLOC 1024 * 1024 * 4 // 4 MiB

#define CERR std::cerr << "Error:\n  "

// Loads a TPU binary image from a file to memory
void loadImageToMemory(tpu::Memory& memory, std::ifstream& handle) {
    // Load address of first instruction
    u8 buf[4] = {0};
    handle.read(reinterpret_cast<char*>(buf), 4);
    u32 startInstAddr = (static_cast<u32>(buf[3]) << 24) |
                        (static_cast<u32>(buf[2]) << 16) |
                        (static_cast<u32>(buf[1]) << 8) |
                        static_cast<u32>(buf[0]);

    std::printf("Address of First Instruction: 0x%08x\n", startInstAddr);

    // Read entire file to memory (guaranteed to be smaller than memory)
    u32 memPtr = 4; // <-- start after first instruction address
    const u32 stepSize = 1024;
    do {
        handle.read( reinterpret_cast<char*>(memory.data() + memPtr), stepSize );
        memPtr += stepSize;
    } while (handle.gcount() > 0);

    // Debug - test for bytes at first instruction 0x0022BEEF
    std::printf("First 4 Instruction Bytes: 0x%08x\n", memory.dword(startInstAddr).dword);
}

int main(int argc, char* argv[]) {
    // Verify args
    if (argc < 2) {
        CERR << "Usage: <tpu> /path/to/image.tpu" << std::endl;
        return EXIT_FAILURE;
    }

    // Verify file exists
    if (!std::filesystem::exists(argv[1])) {
        CERR << "Invalid TPU image path: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream handle( argv[1], std::ios::binary );
    if (!handle.is_open()) {
        CERR << "Failed to open TPU image: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    if (std::filesystem::file_size(argv[1]) > MAX_MEMORY_ALLOC) {
        CERR << "TPU image exceeds memory: " << argv[1] << std::endl;
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

    return EXIT_SUCCESS;
}
