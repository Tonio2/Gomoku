
PYTHON=python

all: run

run: lib
	cd GomokuKivy && ${PYTHON} gomoku.py

lib: lib/pygomoku.so

GomokuEngine/pygomoku.so:
	make -C GomokuEngine pygomoku.so

lib/pygomoku.so: GomokuEngine/pygomoku.so
	mkdir -p lib
	cp -f GomokuEngine/pygomoku.so lib/pygomoku.so

clean:
	make -C GomokuEngine clean

fclean:
	make -C GomokuEngine fclean
	rm -rf lib

re: fclean lib

reTerm: re # Still usefull ?
	cd GomokuKivy && ${PYTHON} gomokuTerminal.py

startTerm: # Still usefull ?
	cd GomokuKivy && ${PYTHON} gomokuTerminal.py

.PHONY: all run lib re reTerm startTerm
