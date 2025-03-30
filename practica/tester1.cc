#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <map>

int main(){
    long num, negsSeguits = 0, max = -1;
    std::optional<long> anterior, anteriorNoNeg;
    std::map <long, long> negsSeguitsMap;
    std::vector<long> llistat (17,0);

    while (std::cin >> num){
        if (num > max) max = num;
        if (num == -1){
            ++negsSeguits;
        }
        else{
            if (negsSeguits){
                negsSeguitsMap[negsSeguits]++;
                negsSeguits = 0;
            }
            
            if (anteriorNoNeg){
                int diff = std::abs(num - anteriorNoNeg.value());
                if (diff < 1) ++llistat[0];
                else if (diff < 2) ++llistat[1];
                else if (diff < 4) ++llistat[2];
                else if (diff < 8) ++llistat[3];
                else if (diff < 16) ++llistat[4];
                else if (diff < 32) ++llistat[5];
                else if (diff < 64) ++llistat[6];
                else if (diff < 128) ++llistat[7];
                else if (diff < 256) ++llistat[8];
                else if (diff < 512) ++llistat[9];
                else if (diff < 1024) ++llistat[10];
                else if (diff < 2048) ++llistat[11];
                else if (diff < 4096) ++llistat[12];
                else if (diff < 8192) ++llistat[13];
                else if (diff < 16384) ++llistat[14];
                else if (diff < 32768) ++llistat[15];
                else ++llistat[16];
            }
            anteriorNoNeg = num; 
        }
        anterior = num;
    }

    for (int i = 0; i < llistat.size(); ++i){
        std::cout << std::setw(5) << std::setfill('0') << (1<<i) << ": " <<  llistat[i] << std::endl;
    }
    std::cout << std::endl;
    
    for(auto it = negsSeguitsMap.cbegin(); it != negsSeguitsMap.cend(); ++it){
        std::cout << it->first << " " << it->second << "\n";
    }
    std::cout << std::endl << std::endl << "max: " << max << std::endl;

}