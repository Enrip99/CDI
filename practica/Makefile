CC=g++
CXXFLAGS=-std=c++20 -o3

default: compress.src/main.cpp compress.src/CompressorPractica.cpp compress.src/DescompressorPractica.cpp compress.src/BinTree.hpp
	$(CC) $(CXXFLAGS) -o compress.cdi compress.src/main.cpp

clean:
	rm -f *.cdi compress.src/*.o