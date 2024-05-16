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


TEST_BINS = $(wildcard ./build/tests/*_tests)


all: $(EXECUTABLE_FILE) $(PY_LIBRARY_FILE)

$(EXECUTABLE_FILE): submodule cmake
	cp -f ./build/$(EXECUTABLE_FILE) $(EXECUTABLE_FILE)

$(PY_LIBRARY_FILE): submodule cmake
	cp -f $(shell find ./build/ -name "PyGomoku.*.so" -type f | xargs ls -t1 | head -n 1) $(PY_LIBRARY_FILE)

build:
	mkdir -p build

submodule:
	git submodule update --init --recursive

cmake: build
	cd build && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) .. && make

install:
	# brew install googletest
	# apt-get install libgtest-dev

	# brew install python@3.12
	# apt-get install python3-dev

run: $(EXECUTABLE_FILE)
	./$(EXECUTABLE_FILE)

test: cmake $(TEST_BINS)

$(TEST_BINS):
	./$@

clean:
	rm -rf build

fclean: clean
	rm -rf $(EXECUTABLE_FILE)
	rm -rf $(PY_LIBRARY_FILE)

re: fclean all

.PHONY: all submodule cmake install run test $(TEST_BINS) clean
