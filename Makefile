objects = main.o wbptree.o
test: $(objects)
	gcc -g -o test  $(objects)
main.o: main.c wbptree.h
	gcc -g -c main.c wbptree.h -lrt -lm
WBPlusTree.o: wbptree.c wbptree.h
	gcc -g -c wpbtree.c wbptree.h
.PHONY: clean
clean:
	-rm test $(objects)
