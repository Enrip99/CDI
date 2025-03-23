#include <stdint.h>
#include <iostream>

int main(){
    uint8_t*  arr = new uint8_t[32768];
    for (int i = 0; i < 32768; ++i){
        arr[i] = (uint8_t) i;
    }

    for (int i = 0; i < 32768; ++i){
        std::cout << (int) arr[i] << std::endl;
    }
}