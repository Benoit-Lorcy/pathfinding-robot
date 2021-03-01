prog.exe:main.o function.o graphique.o 
	gcc main.o function.o graphique.o -g -o prog.exe -lSDL2
graphique.o:graphique.c function.h graphique.h
	gcc -g -c graphique.c
function.o:function.c function.h graphique.h
	gcc -g -c function.c
main.o:main.c function.h graphique.h
	gcc -g -c main.c
clean:
	rm -rf *.o