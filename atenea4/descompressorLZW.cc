#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>

int main(){
    std::string codi, diccionariInicial, sortida;
    std::vector <std::string> diccionari;
    std::getline(std::cin, codi);
    std::getline(std::cin, diccionariInicial);

    std::regex expChars("'(.)'");
    std::smatch res;
    std::string::const_iterator searchStart( diccionariInicial.cbegin() );
    while (regex_search( searchStart, diccionariInicial.cend(), res, expChars ) ){
        searchStart = res.suffix().first;
        diccionari.push_back(res.str(1));
    }

    std::regex expNums("\\d+");
    std::string::const_iterator searchStartNum( codi.cbegin() );
    std::string anteriorParaula = "";
    while (regex_search( searchStartNum, codi.cend(), res, expNums ) ){

        searchStartNum = res.suffix().first;
        int num = std::stoi(res.str(0));
        //std::cout << num;
        if (num--){
            std::string paraulaActual;
            if (num < diccionari.size()){
                // És al diccionari
                paraulaActual = diccionari[num];
                if (anteriorParaula != ""){
                    diccionari.push_back(anteriorParaula + paraulaActual[0]);
                }
                anteriorParaula = paraulaActual;
                sortida += paraulaActual;
            }
            else{
                paraulaActual = anteriorParaula + anteriorParaula[0];
                sortida += paraulaActual;
                diccionari.push_back(paraulaActual);
                anteriorParaula = paraulaActual;
            }
        }
    }
    std::cout << sortida << std::endl;

    //for (int i = 0; i < diccionari.size(); ++i) std::cout << diccionari[i] << std::endl;

}