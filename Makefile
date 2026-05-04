BUILD_DEBUG		:= build/debug
BUILD_RELEASE	:= build/release

CMAKE_ARGS		:= -DFETCHCONTENT_SOURCE_DIR_KITSUNE=$(HOME)/Projects/kitsune

CORES			?= $(shell nproc 2>/dev/null || echo 4)

.PHONY: all debug release runt rune update clean clean-full help

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

runt: debug
	@if [ -z "$(NAME)" ]; then echo "Specify NAME"; exit 1; fi
	@echo "Running Test '$(NAME)'..."
	@./$(BUILD_DEBUG)/test/test_$(NAME)

rune: debug
	@if [ -z "$(NAME)" ]; then echo "Specify NAME"; exit 1; fi
	@echo "Running Example '$(NAME)'..."
	@./$(BUILD_DEBUG)/example/example_$(NAME)

update:
	@echo "Updating dependencies..."
	@touch CMakeLists.txt
	@$(MAKE) debug

clean:
	@if [ -n "$(NAME)" ]; then \
		echo "Cleaning target '$(NAME)'..."; \
		rm -rf $(BUILD_DEBUG)/test/CMakeFiles/test_$(NAME).dir; \
		rm -rf $(BUILD_DEBUG)/example/CMakeFiles/example_$(NAME).dir; \
		rm -f $(BUILD_DEBUG)/test/test_$(NAME) $(BUILD_DEBUG)/example/example_$(NAME); \
	else \
		echo "Cleaning local files..."; \
		rm -rf $(BUILD_DEBUG)/test $(BUILD_DEBUG)/example; \
		rm -rf $(BUILD_RELEASE)/test $(BUILD_RELEASE)/example; \
	fi

clean-full:
	@echo "Cleaning build..."
	@rm -rf build

help:
	@echo "Usage:"
	@echo "  make                        - Build debug version"
	@echo "  make debug                  - Build debug version"
	@echo "  make release                - Build release version"
	@echo "  make update                 - Re-fetch/update dependencies"
	@echo "  make clean [NAME=x]         - Remove local object files"
	@echo "  make clean-full             - Remove all build files"