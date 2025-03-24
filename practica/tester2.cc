#include <stdio.h>
#include <vector>
#include <iostream>
#include <filesystem>

int main(int argc, char ** argv){
    std::filesystem::path p{argv[1]};

    FILE * entrada;
    entrada = fopen (argv[1], "r");
    __uint8_t lectura[2048];
    std::vector<long> count (256,0);
    int max;
    while (max = fread(lectura, sizeof(__uint8_t), 2048, entrada)){
        for (int i = 0; i < max; ++i){
            count[lectura[i]]++;
        }
    }

    for (int i = 0; i < 256; ++i ){
        std::cout << count[i] << " " << i << std::endl;
    }

}