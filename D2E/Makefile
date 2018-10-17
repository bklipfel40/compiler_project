YFLAGS = -d
CFLAGS = -g  -std=gnu11
LOADLIBES = -ll -ly

D2E.o: 		D2E.y
Scanner.o:	Scanner.l

D2E: 		D2E.o Scanner.o

clean:
	rm D2E.o Scanner.o D2E