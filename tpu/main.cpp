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
    // Read kernel and text segment start offsets
    u32 kernelLen, textLen;
    handle.read( reinterpret_cast<char*>(&kernelLen), 4 );
    handle.read( reinterpret_cast<char*>(&textLen), 4 );

    if (kernelLen > KERNEL_IMAGE_MAX_SIZE)
        throw std::runtime_error("Kernel image is too large.");

    if (textLen > USER_IMAGE_MAX_SIZE)
        throw std::runtime_error("User program is too large.");

    // Read kernel program (& its data segment after)
    u32 memPtr = IMAGE_START_ADDR;
    for (u32 i = 0; i < kernelLen; ++i) {
        handle.read( reinterpret_cast<char*>(memory.data() + memPtr), 1 );
        u32 bytesRead = handle.gcount();
        memPtr += bytesRead;

        if (bytesRead == 0)
            throw std::runtime_error("Unexpected EOF while reading kernel image.");
    }

    // Read user program (& its data segment after)
    memPtr = USER_SPACE_START;
    for (u32 i = 0; i < textLen; ++i) {
        handle.read( reinterpret_cast<char*>(memory.data() + memPtr), 1 );
        u32 bytesRead = handle.gcount();
        memPtr += bytesRead;

        if (bytesRead == 0)
            throw std::runtime_error("Unexpected EOF while reading user program.");
    }
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
        handle.close();
        return EXIT_FAILURE;
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
