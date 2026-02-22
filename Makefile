.PHONY: init all

# Build targets
BIN_TPU := bin/tpu
BIN_TASM := bin/tasm

# Sources
HDR_TPU := $(shell find ./tpu/ -type f -name "*.hpp")
SRC_TPU := $(shell find ./tpu/ -type f -name "*.cpp")

all: tpu tasm

################################################################
########################## TPU Itself ##########################
################################################################

tpu: $(BIN_TPU)
$(BIN_TPU): $(HDR_TPU) $(SRC_TPU)
	@echo "Building the TPU..."
	@g++ \
		$(SRC_TPU) \
		-o $(BIN_TPU) \
		-std=c++20 \
		-Wall -Wextra -Werror -g
	@echo "✅ Done."

################################################################
######################## TPU Assembler #########################
################################################################

tasm: $(BIN_TASM)
$(BIN_TASM):
	@echo "Symlinking the TPU Assembler..."
	@ln -sf $(shell pwd)/tasm/tasm.py $(BIN_TASM)
	@chmod +x $(BIN_TASM)
	@echo "✅ Done."

################################################################
############################ Misc. #############################
################################################################

init:
	@mkdir -p bin
	@echo "Environment configured."
