YFLAGS = -d
CFLAGS = -g  -std=gnu11
LOADLIBES = -ll -ly

ExprEval.o: 	ExprEval.y
Scanner.o:	Scanner.l

ExprEval: 	ExprEval.o Scanner.o

clean:
		rm *.o ExprEval
