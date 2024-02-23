all:
	git submodule update --init --recursive
	mkdir -p lib GomokuEngine/build
	cd GomokuEngine/build && cmake .. && make
	mv GomokuEngine/build/pygomoku.cpython-310-x86_64-linux-gnu.so lib/pygomoku.so
	cd GomokuKivy && python gomoku.py

re:
	cd GomokuEngine/build && make
	mv GomokuEngine/build/pygomoku.cpython-310-x86_64-linux-gnu.so lib/pygomoku.so
	cd GomokuKivy && python gomoku.py