app1 =  Exec/ExemploMultiplicacao
app2 =  Exec/ExemploMandelbrot_Vector
app3 =  Exec/ExemploMandelbrot_Image2D
comp = clang++

all:Multiplicacao.o Mandelbrot_Vector.o Mandelbrot_Image2D.o
	clear
	$(comp) Multiplicacao.o -o $(app1) -O3  -lOpenCL
	$(comp) Mandelbrot_Vector.o -o $(app2) -O3  -lOpenCL
	$(comp) Mandelbrot_Image2D.o -o $(app3) -O3  -lOpenCL
	./$(app1)
	./$(app2)
	./$(app3)

Multiplicacao.o: Exemplos/Multiplicacao.cpp
	$(comp) -c Exemplos/Multiplicacao.cpp -o Multiplicacao.o  -O3

Mandelbrot_Vector.o: Exemplos/Mandelbrot_Vector.cpp
	$(comp) -c Exemplos/Mandelbrot_Vector.cpp -o Mandelbrot_Vector.o  -O3	

Mandelbrot_Image2D.o: Exemplos/Mandelbrot_Image2D.cpp
	$(comp) -c Exemplos/Mandelbrot_Image2D.cpp -o Mandelbrot_Image2D.o  -O3	

clean:
	clear
	rm -f *.o
	rm -f Exec/*