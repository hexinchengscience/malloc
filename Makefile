CC=gcc

SRCS := $(wildcard *.c)

main:$(SRCS)
	$(CC) -g -o main $(SRCS)
	
clean:
	rm  main