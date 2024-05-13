
NAME = Gomoku

all: $(NAME)

$(NAME):
	pyinstaller --onefile gomoku.py
	cp dist/gomoku ./$(NAME)

clean:
	rm -rf build dist gomoku.spec

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)
