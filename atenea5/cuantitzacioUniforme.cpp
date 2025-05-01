#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

unsigned char * bufer;

unsigned char agafa_bits(int quantitat){
    static int posicio = 0, bit = 0;
    unsigned char resultat = 0;
    while (quantitat--){
        resultat <<= 1;
        if (bufer[posicio] & 1 << (7 - bit++)) ++resultat;
        if (bit = 8) {
            bit = 0;
            ++posicio;
        }
    }
    return resultat;
}

int main (int argc, char ** argv){
    if (argc != 3) return -1;
    int fitxer_entrada, fitxer_sortida;
    fitxer_entrada = open(argv[1], O_RDONLY);
    fitxer_sortida = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fitxer_entrada == -1 || fitxer_sortida == -1) return -1;

    int resultat;
    
    unsigned char opcions [6];

    resultat = read(fitxer_entrada, opcions, 6);
    if (resultat == -1) return -1;

    unsigned long n = opcions[0] << 8 | opcions[1]; //fileres
    unsigned long m = opcions[2] << 8 | opcions[3]; //columnes
    unsigned char n_b = opcions[4]; //mida de les fileres i columnes dels blocs
    unsigned char b = opcions[5]; //bits per pixel del bloc
    std::vector <std::vector <unsigned char>> imatge (n, std::vector<unsigned char> (m)); //imatge final
    unsigned int num_blocs = (n/n_b)*(m/n_b); //numero de blocs
    unsigned int mida_bloc = n_b * n_b * b; //Nombre de bits en un bloc
    unsigned int nivells = 1 << n_b; //Nombre de valors diferents que pot tenir un pixel d'un bloc
    unsigned int bytes_totals = (16 * num_blocs) + ((mida_bloc * n_b * num_blocs)/8);

    bufer = (unsigned char *) malloc(bytes_totals);
    resultat = read(fitxer_entrada, bufer, bytes_totals);
    if (resultat == -1) return -1;

    std::vector <unsigned char> bloc (n_b * n_b);
    std::vector <int> escales (nivells);

    for (int i = 0; i < num_blocs; ++i){
        unsigned int min = agafa_bits(8);
        unsigned int max = agafa_bits(8);
        float escala = (max - min) / (float) nivells;
        for (int j = 0; j < nivells; ++j) escales [j] = min + (escala * j) + (escala / 2);

        for (int j = 0; j < n_b * n_b; ++j) bloc[j] = escales [agafa_bits(b)];

        

    }
    
}