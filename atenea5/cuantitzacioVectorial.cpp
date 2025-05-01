#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>
#include <vector>

#include <iostream>

unsigned char * bufer_entrada;

unsigned int agafa_bits(int quantitat){
    static int posicio = 0, bit = 0;
    unsigned int resultat = 0;
    while (quantitat--){
        resultat <<= 1;
        if (bufer_entrada[posicio] & (1 << (7 - bit++))) ++resultat;
        if (bit == 8) {
            bit = 0;
            ++posicio;
        }
    }
    return resultat;
}

unsigned int bitsPerRepresentar(int input){
    unsigned int target = 0;
    while (input >>= 1) ++target;
    return target + 1;
}

int main (int argc, char ** argv){

    if (argc != 3) return -1;
    int fitxer_entrada, fitxer_sortida;
    fitxer_entrada = open(argv[1], O_RDONLY);
    fitxer_sortida = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fitxer_entrada == -1 || fitxer_sortida == -1) return -1;

    int resultat;
    
    unsigned char opcions [7];

    resultat = read(fitxer_entrada, opcions, 7);
    if (resultat == -1) return -1;


    unsigned long n = opcions[0] << 8 | opcions[1]; //fileres
    unsigned long m = opcions[2] << 8 | opcions[3]; //columnes
    unsigned char n_b = opcions[4]; //mida de les fileres i columnes dels blocs
    unsigned long e = opcions[5] << 8 | opcions[6]; //nombre d'entrades al diccionari
    unsigned char imatge [n][m];
    unsigned long fileres_de_blocs = n / n_b;
    unsigned long columnes_de_blocs = m / n_b;
    unsigned int mida_index = bitsPerRepresentar(e);
    unsigned int num_blocs = fileres_de_blocs * columnes_de_blocs; //numero de blocs
    unsigned char diccionari [e][n_b][n_b];
    unsigned int bytes_totals = ((mida_index * num_blocs)/8) + (((mida_index * num_blocs) % 8) != 0);

    resultat = read(fitxer_entrada, & diccionari, e * n_b * n_b);
    if (resultat == -1) return -1;

    bufer_entrada = (unsigned char *) malloc(bytes_totals);
    resultat = read(fitxer_entrada, bufer_entrada, bytes_totals);
    if (resultat == -1) return -1;

    for (int i = 0; i < num_blocs; ++i){
        int fil = (i % columnes_de_blocs) * n_b;
        int col = (i / columnes_de_blocs) * n_b;
        unsigned int index = agafa_bits(mida_index);
        for (int j = 0; j < n_b; ++j) for (int k = 0; k < n_b; ++k) imatge[col+j][fil+k] = diccionari[index][j][k];
    }

    char preamble [256];
    resultat = sprintf(preamble, "P5\x0A# Creat per Enric Carpintero Rico per a l'entrega 5 de CDI.\x0A%ld\x0A%ld\x0A" "255\x0A", m, n);
    if (resultat < 0) return -1;
    resultat = write(fitxer_sortida, &preamble, resultat);
    if (resultat == -1) return -1;

    resultat = write(fitxer_sortida, &imatge[0][0], n * m);
    if (resultat == -1) return -1;

    free(bufer_entrada);
    close(fitxer_entrada);
    close(fitxer_sortida);
    
}