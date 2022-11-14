default:
	rm -rf dist/
	mkdir -p dist/
	cp font.ttf dist/
	g++ ./src/main.cpp -o ./dist/main -lsfml-graphics -lsfml-window -lsfml-system -std=c++11 -pthread
	./dist/main

preinstall:
	#ubuntu
	sudo apt update
	sudo apt install libsfml-dev build-essential cmake 