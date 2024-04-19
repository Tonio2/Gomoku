
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LIBNAME:=pygomoku.cpython-310-x86_64-linux-gnu.so
endif
ifeq ($(UNAME_S),Darwin)
	LIBNAME:=pygomoku.cpython-312-darwin.so
endif

PYTHON=python

all:
	git submodule update --init --recursive
	mkdir -p lib GomokuEngine/build
	cd GomokuEngine/build && cmake .. && make
	mv GomokuEngine/build/${LIBNAME} lib/pygomoku.so
	cd GomokuKivy && ${PYTHON} gomoku.py

re:
	cd GomokuEngine/build && make
	mv GomokuEngine/build/${LIBNAME} lib/pygomoku.so
	cd GomokuKivy && ${PYTHON} gomoku.py

reTerm:
	cd GomokuEngine/build && make
	mv GomokuEngine/build/${LIBNAME} lib/pygomoku.so
	cd GomokuKivy && ${PYTHON} gomokuTerminal.py

startTerm:
	cd GomokuKivy && ${PYTHON} gomokuTerminal.py

start:
	cd GomokuKivy && ${PYTHON} gomoku.py

build:
	cd GomokuEngine/build && make
	mv GomokuEngine/build/${LIBNAME} lib/pygomoku.so