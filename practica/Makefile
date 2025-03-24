CC=g++
CXXFLAGS=-std=c++20

all: compressor descompressor

compressor: compress.src/CompressorPractica.cpp compress.src/BinTree.hpp
	g++ -o compress.cdi compress.src/CompressorPractica.cpp

descompressor: compress.src/DescompressorPractica.cpp compress.src/BinTree.hpp
	g++ -o decompress.cdi compress.src/DescompressorPractica.cpp

clean:
	rm -f *.cdi compress.src/*.o