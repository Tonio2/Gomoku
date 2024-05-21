
NAME = Gomoku

all: $(NAME)

run: resources
	@rm -f lib/pygomoku.so && make -C GomokuEngine pygomoku.so
	@make lib/pygomoku.so
	python GomokuKivy/gomoku.py

lib_dir:
	mkdir -p lib

lib: lib_dir
	make -C GomokuEngine lib
	cp -f GomokuEngine/pygomoku.so lib/pygomoku.so

GomokuKivy/Gomoku: lib/pygomoku.so
	make -C GomokuKivy Gomoku

resources:
	cp -r GomokuKivy/resources .

$(NAME): GomokuKivy/Gomoku resources
	cp -f GomokuKivy/Gomoku ./$(NAME)

clean:
	make -C GomokuEngine clean
	make -C GomokuKivy clean

fclean:
	make -C GomokuEngine fclean
	make -C GomokuKivy fclean
	rm -rf lib resources
	rm -f $(NAME)

re: fclean $(NAME)

re_lib:
	rm -rf lib
	mkdir -p lib
	make -C GomokuEngine re_lib
	cp -f GomokuEngine/pygomoku.so lib/pygomoku.so

.PHONY: all run lib re reTerm startTerm
