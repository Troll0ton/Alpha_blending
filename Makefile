compile: main.cpp
	g++ -c -Ofast -mavx -mavx512dq main.cpp bmp_utils.cpp alpha_blending.cpp
	g++ main.o bmp_utils.o alpha_blending.o -o app -lsfml-graphics -lsfml-window -lsfml-system

run:
	./app