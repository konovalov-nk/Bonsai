# Bonsai build/run helper
#
#   make               build the visual demo   -> build/bonsai2
#   make run-visual    build and run the demo with PRIME offload
#   make headless      build the headless code -> build-headless/bonsai2_slowdust
#   make run-headless  build and run headless
#   make clean         remove both build directories
#
# Useful overrides:
#   make CUDA_ARCH=120 BUILD_TYPE=Release JOBS=4
#   make run-visual PARAMS=presets/screen-1794842753.txt
#   make run-visual VISUAL_ARGS="-T 5000 --reducebodies 4"
#   make run-headless HEADLESS_ARGS="-T 10"

SHELL := /bin/bash

SRC_DIR            := runtime
BUILD_DIR          ?= build
HEADLESS_BUILD_DIR ?= build-headless
BUILD_TYPE         ?= Release
INPUT              ?= model3_child_compact.tipsy
PARAMS             ?= presets/screen-1794842753.txt
COLORPARAMS         ?=
PRESET_DIR         ?= presets
IC_DIR             ?= inputExamples
VISUAL_ARGS        ?= -T 1000
HEADLESS_ARGS      ?= -T 1

PRESET_DIR_ARGS := --presetdir $(abspath $(PRESET_DIR))
IC_DIR_ARGS := --icdir $(abspath $(IC_DIR))
PRESET_ARGS := $(if $(PARAMS),--params $(abspath $(PARAMS)),) \
               $(if $(COLORPARAMS),--colorparams $(abspath $(COLORPARAMS)),)

# FindCUDA has a race in its generated .depend files with parallel
# "Unix Makefiles" builds. Ninja handles those dependencies correctly.
ifeq ($(shell command -v ninja 2>/dev/null),)
GENERATOR ?= Unix Makefiles
JOBS      ?= 1
else
GENERATOR ?= Ninja
JOBS      ?= 8
endif

# Build inside the repository: the demo looks for ../images relative to its
# working directory, and build/../images is the repository's images/ directory.
DETECTED_ARCH := $(shell nvidia-smi --query-gpu=compute_cap --format=csv,noheader 2>/dev/null | head -n1 | tr -d '.')
CUDA_ARCH     ?= $(if $(DETECTED_ARCH),$(DETECTED_ARCH),120)

# CUDA 13 does not officially support every new host compiler yet; prefer gcc-15.
HOST_CXX ?= $(shell command -v g++-15 2>/dev/null || command -v g++)
HOST_CC  ?= $(shell command -v gcc-15  2>/dev/null || command -v gcc)

CUDA_NVCC_FLAGS := -gencode;arch=compute_$(CUDA_ARCH),code=sm_$(CUDA_ARCH)

CMAKE_FLAGS := \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DCMAKE_C_COMPILER=$(HOST_CC) \
	-DCMAKE_CXX_COMPILER=$(HOST_CXX) \
	"-DCUDA_NVCC_FLAGS=$(CUDA_NVCC_FLAGS)"

.PHONY: all visual headless configure headless-configure \
        run run-visual run-headless clean distclean

all: visual

configure:
	cmake -G "$(GENERATOR)" -S $(SRC_DIR) -B $(BUILD_DIR) \
		$(CMAKE_FLAGS) \
		-DUSE_MPI=OFF -DUSE_CUB=0 -DUSE_DUST=1 -DUSE_OPENGL=1

visual: configure
	cmake --build $(BUILD_DIR) -j$(JOBS)

run-visual: visual
	cd $(BUILD_DIR) && \
		__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia \
		./bonsai2 -i $(INPUT) --dev 0 --renderdev 0 $(PRESET_DIR_ARGS) $(IC_DIR_ARGS) $(PRESET_ARGS) $(VISUAL_ARGS)

run: run-visual

headless-configure:
	cmake -G "$(GENERATOR)" -S $(SRC_DIR) -B $(HEADLESS_BUILD_DIR) \
		$(CMAKE_FLAGS) \
		-DUSE_MPI=OFF -DUSE_CUB=0 -DUSE_DUST=0 -DUSE_OPENGL=OFF

headless: headless-configure
	cmake --build $(HEADLESS_BUILD_DIR) -j$(JOBS)

run-headless: headless
	cd $(HEADLESS_BUILD_DIR) && \
		./bonsai2_slowdust -i $(INPUT) --dev 0 $(HEADLESS_ARGS)

clean:
	rm -rf $(BUILD_DIR) $(HEADLESS_BUILD_DIR)

distclean: clean
