LOADLIBES = -ll -ly

AST.o: 		  AST.y PrintTree.h
Scanner.o:	Scanner.l PrintTree.h

AST: 		    AST.o Scanner.o PrintTree.o

test:
				./AST < test.src > test.html

clean:
			rm *.o AST
