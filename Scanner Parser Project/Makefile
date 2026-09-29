all: scanit.exe parseit.exe

token.o: token.c token.h
	gcc -c token.c

scan.o: scan.c scan.h token.h
	gcc -c scan.c

parse.o: parse.c parse.h scan.h token.h
	gcc -c parse.c

scanit.o: scanit.c scan.h token.h
	gcc -c scanit.c

parseit.o: parseit.c parse.h scan.h token.h
	gcc -c parseit.c

scanit.exe: scanit.o token.o scan.o
	gcc -o scanit.exe scanit.o token.o scan.o

parseit.exe: parseit.o token.o parse.o scan.o
	gcc -o parseit.exe parseit.o token.o parse.o scan.o

clean:
	rm -f *.exe *.o