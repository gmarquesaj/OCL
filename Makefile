app1 =  Exec/ExemploMultiplicacao
app2 =  Exec/ExemploMandelbrot
comp = clang++

all:Multiplicacao.o Mandelbrot.o
	$(comp) Multiplicacao.o -o $(app1) -O3  -lOpenCL
	$(comp) Mandelbrot.o -o $(app2) -O3  -lOpenCL
	./$(app1)
	./$(app2)

Multiplicacao.o: Exemplos/Multiplicacao.cpp
	$(comp) -c Exemplos/Multiplicacao.cpp -o Multiplicacao.o  -O3

Mandelbrot.o: Exemplos/Mandelbrot.cpp
	$(comp) -c Exemplos/Mandelbrot.cpp -o Mandelbrot.o  -O3	

clean:
	clear
	rm -f *.o
	rm -f Exec/*