build:
	rm -rf dist/
	mkdir -p ./dist/
	cp font.ttf dist/
	g++ ./TronSocketGame/src/main.cpp -o ./dist/main -lsfml-graphics -lsfml-window -lsfml-system -std=c++11 -pthread
	g++ ./Servidor/server.cpp   -o ./dist/server
game: 
	./dist/main
server: 
	./dist/server

preinstall:
	#ubuntu
	sudo apt update
	sudo apt install libsfml-dev build-essential cmake 