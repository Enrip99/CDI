CC=g++
CXXFLAGS=-std=c++11

all: compress_normal descompress_normal compress_adaptatiu descompress_adaptatiu

compress_normal: CompressorNormal.o EscriptorBits.o EscriptorBits.hpp DimensionsIO.hpp BinTree.hpp
	$(CC) -o CompressorNormal CompressorNormal.o EscriptorBits.o

descompress_normal: DescompressorNormal.o LectorBits.o LectorBits.hpp DimensionsIO.hpp BinTree.hpp
	$(CC) -o DescompressorNormal DescompressorNormal.o LectorBits.o

compress_adaptatiu: CompressorAdaptatiu.o EscriptorBits.o EscriptorBits.hpp DimensionsIO.hpp BinTree.hpp
	$(CC) -o CompressorAdaptatiu CompressorAdaptatiu.o EscriptorBits.o

descompress_adaptatiu: DescompressorAdaptatiu.o LectorBits.o LectorBits.hpp DimensionsIO.hpp BinTree.hpp
	$(CC) -o DescompressorAdaptatiu DescompressorAdaptatiu.o LectorBits.o

clean: 
	rm -f *.o CompressorNormal DescompressorNormal CompressorAdaptatiu DescompressorAdaptatiu