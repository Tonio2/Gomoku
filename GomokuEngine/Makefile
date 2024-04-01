-include .env
export

EXECUTABLE_FILE=gktool
PY_LIBRARY_FILE=pygomoku.so

ifeq ($(BUILD_TYPE),DEBUG)
	BUILT_EXEC_FILE := $(EXECUTABLE_FILE)-debug
else
	BUILT_EXEC_FILE := $(EXECUTABLE_FILE)
endif

ifeq ($(TIMER),OFF)
	CXXFLAGS += -DNOTIMER
endif

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	BUILT_PYLIB_FILE := PyGomoku.cpython-310-x86_64-linux-gnu.so
endif
ifeq ($(UNAME_S),Darwin)
	BUILT_PYLIB_FILE := PyGomoku.cpython-312-darwin.so
endif

TEST_BINS = $(wildcard ./build/tests/*_tests)



all: $(EXECUTABLE_FILE) $(PY_LIBRARY_FILE)

$(EXECUTABLE_FILE): cmake
	cp -f ./build/$(BUILT_EXEC_FILE) $(EXECUTABLE_FILE)

$(PY_LIBRARY_FILE): cmake
	cp -f ./build/$(BUILT_PYLIB_FILE) $(PY_LIBRARY_FILE)

build:
	mkdir -p build

submodule:
	git submodule update --init --recursive

cmake: build submodule
	cd build && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) .. && make

install:
	# brew install gtest
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
