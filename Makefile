BUILD_DEBUG		:= build/debug
BUILD_RELEASE	:= build/release

CMAKE_ARGS		:= 

CORES			?= $(shell nproc 2>/dev/null || echo 4)

.PHONY: all debug release clean-full help

all: debug

$(BUILD_DEBUG)/CMakeCache.txt: CMakeLists.txt
	@echo "Configuring Debug build..."
	@mkdir -p $(BUILD_DEBUG)
	@cmake -S . -B $(BUILD_DEBUG) -DCMAKE_BUILD_TYPE=Debug $(CMAKE_ARGS)

$(BUILD_RELEASE)/CMakeCache.txt: CMakeLists.txt
	@echo "Configuring Release build..."
	@mkdir -p $(BUILD_RELEASE)
	@cmake -S . -B $(BUILD_RELEASE) -DCMAKE_BUILD_TYPE=Release $(CMAKE_ARGS)

debug: $(BUILD_DEBUG)/CMakeCache.txt
	@echo "Building Debug..."
	@cmake --build $(BUILD_DEBUG) -j $(CORES)

release: $(BUILD_RELEASE)/CMakeCache.txt
	@echo "Building Release..."
	@cmake --build $(BUILD_RELEASE) -j $(CORES)

clean-full:
	@echo "Cleaning build..."
	@rm -rf build

help:
	@echo "Usage:"
	@echo "  make                        - Build debug version"
	@echo "  make debug                  - Build debug version"
	@echo "  make release                - Build release version"
	@echo "  make clean-full             - Remove all build files"