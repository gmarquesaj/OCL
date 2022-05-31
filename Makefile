app =  app
comp = clang++
all: main.o
	$(comp) main.o -o $(app) -O3  -lOpenCL
	./$(app)
main.o: main.cpp
	$(comp) -c main.cpp -o main.o  -O3
clean:
	clear
	rm -f *.o
