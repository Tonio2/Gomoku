-include .env
export

EXECUTABLE_FILE=gktool
PY_LIBRARY_FILE=pygomoku.so

ifeq ($(BUILD_TYPE),DEBUG)
	CXXFLAGS += -pg
else
	# CXXFLAGS += -march=native -msse4.2
endif

ifeq ($(TIMER),OFF)
	CXXFLAGS += -DNOTIMER
endif

ifeq ($(LOGGING),ON)
	CXXFLAGS += -DLOGGING
endif

ifeq ($(OS),Windows_NT)
	detected_OS := Windows
else
	detected_OS := $(shell uname)
endif

ifeq ($(detected_OS),Linux)
	JOBS := $(shell nproc)
else ifeq ($(detected_OS),Darwin)  # macOS, typically
	JOBS := $(shell sysctl -n hw.ncpu)
else
	JOBS := 1
endif


TEST_BINS = $(wildcard ./build/tests/*_tests)


all: $(EXECUTABLE_FILE) lib

$(EXECUTABLE_FILE): submodule cmake_gktool
	cp -f ./build/$(EXECUTABLE_FILE) $(EXECUTABLE_FILE)

lib: submodule cmake_pygomoku
	cp -f $(shell find ./build/ -name "PyGomoku.*.so" -type f | xargs ls -t1 | head -n 1) $(PY_LIBRARY_FILE)

build:
	mkdir -p build

submodule:
	git submodule update --init --recursive

cmake: build
	cd build && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ..

cmake_gktool: cmake
	cd build && make -j$(JOBS) $(EXECUTABLE_FILE)

cmake_pygomoku: cmake
	cd build && make -j$(JOBS) PyGomoku

cmake_test: cmake
	cd build && make -j$(JOBS)

install:
	# brew install googletest
	# apt-get install libgtest-dev

	# brew install python@3.12
	# apt-get install python3-dev

run: $(EXECUTABLE_FILE)
	./$(EXECUTABLE_FILE)

test: cmake_test
	@make $(TEST_BINS)

$(TEST_BINS):
	./$@

clean:
	rm -rf build

fclean: clean
	rm -rf $(EXECUTABLE_FILE)
	rm -rf $(PY_LIBRARY_FILE)

re: fclean all

fclean_lib: clean
	rm -rf $(PY_LIBRARY_FILE)

re_lib: fclean lib

fclean_gktool: fclean $(EXECUTABLE_FILE)

re_gktool: fclean $(EXECUTABLE_FILE)

.PHONY: all submodule cmake install run test $(TEST_BINS) clean
