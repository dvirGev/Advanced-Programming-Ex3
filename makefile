.PHONY: all clean
all: stree

stree:	stree.c
	gcc stree.c -o stree

clean:
	-rm stree