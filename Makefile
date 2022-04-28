TARGET = fifo round-robin

all: ${TARGET}

fifo: fifo.cpp
	g++ fifo.cpp queue.cpp -o fifo -pthread -O2 -I.

round-robin: round-robin.cpp
	g++ round-robin.cpp queue.cpp -o round-robin -pthread -O2 -I.

.PHONY: clean
clean:
	rm -f *.o ${TARGET}
