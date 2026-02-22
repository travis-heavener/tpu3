#include <atomic>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <signal.h>

#include "defines.hpp"
#include "memory.hpp"
#include "tpu.hpp"

#define CERR std::cerr << "Error:\n  "

using namespace tpu;

/******************** START SIGNAL HANDLERS ********************/

// Global termination flag (atomic)
std::atomic<bool> isExiting{false};

void catchSig(int) {
    isExiting.store(true);
}

void initSigHandler() {
    struct sigaction sigIntHandler;
    
    sigIntHandler.sa_handler = catchSig;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    
    sigaction(SIGINT, &sigIntHandler, NULL);
    sigaction(SIGTERM, &sigIntHandler, NULL);
}

/******************** END SIGNAL HANDLERS ********************/

// Loads a TPU binary image from a file to memory
void loadImageToMemory(tpu::Memory& memory, std::ifstream& handle) {
    // Read entire file to memory (guaranteed to be smaller than memory)
    u32 memPtr = IMAGE_START_ADDR;
    const u32 stepSize = 1024;
    do {
        handle.read( reinterpret_cast<char*>(memory.data() + memPtr), stepSize );
        memPtr += stepSize;
    } while (handle.gcount() > 0);
}

int main(int argc, char* argv[]) {
    initSigHandler();

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

    if (std::filesystem::file_size(argv[1]) > MAX_IMAGE_SIZE) {
        CERR << "TPU image exceeds maximum allowed size: " << argv[1] << std::endl;
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
        tpu.start(memory, isExiting);
    } catch (tpu::Exception& e) {
        std::cerr << e.what() << std::endl;

        // Dump registers
        tpu.dumpRegs();
    }

    // Dump registers
    tpu.dumpRegs();

    std::cout << "Killed TPU." << std::endl;

    return EXIT_SUCCESS;
}
