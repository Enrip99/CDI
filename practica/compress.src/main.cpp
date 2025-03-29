#include "CompressorPractica.cpp"
#include "DescompressorPractica.cpp"
#include <string.h>

void usage(char * comanda){
    std::string misError (comanda);
    std::cerr << "Arguments incorrectes. Ús: " << std::endl <<
        "  " + misError + " -c entrada.txt sortida.knk" << std::endl <<
        "  " + misError + " -d entrada.knk sortida.txt" << std::endl;
    exit(1);
}

int main (int argc, char ** argv){
    if (argc != 4) usage(argv[0]);

    if (strcmp(argv[1], "-d") == 0) {
        DescompressorPrac d;
        d.descomprimeix (argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "-c") == 0) {
        CompressorPrac c;
        c.comprimeix (argv[2], argv[3]);
    }
    else usage(argv[0]);

}