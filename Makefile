CC=g++
CXXFLAGS=-I./include -I/usr/local/include -std=c++11
LIBS=-lntl -lgmpxx -lgmp -lcryptopp -lstdc++ -lpthread -fopenmp -O3
DEPS=./include/bloom_filter.hpp
DEFS=-D__DEBUG
OBJS=Rand.o Hashtable.o Polynomial.o Server.o Client.o util.o test.o

%.o: %.c $(DEPS)
	$(CC) -c  $@ $< $(CXXFLAGS)

test: $(OBJS)
	$(CC) -o  $@ $^ $(LIBS) 

test-rns: $(OBJS)
	$(CC) -o  $@ $^ $(LIBS) 

.PHONY: clean

clean:
	rm -rf *.o test
