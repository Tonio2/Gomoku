
NAME = Gomoku

all: $(NAME)

run: resources
	@rm -f lib/pygomoku.so && make -C GomokuEngine pygomoku.so
	@make lib/pygomoku.so
	python GomokuKivy/gomoku.py

GomokuEngine/pygomoku.so:
	make -C GomokuEngine pygomoku.so

lib:
	mkdir -p lib

lib/pygomoku.so: lib GomokuEngine/pygomoku.so
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

.PHONY: all run lib re reTerm startTerm
