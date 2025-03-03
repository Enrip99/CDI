#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <stdio.h>
#include "EscriptorBits.hpp"
#include "DimensionsIO.hpp"
#include "BinTree.hpp"

#define estructuraParella std::pair<__uint8_t, unsigned long long int>
#define estructuraArbre BinTree < estructuraParella >
#define estructuraCache std::map<__uint8_t, std::vector<bool> >

bool dirty;

estructuraArbre getLiderBloc(estructuraArbre root, estructuraArbre objectiu){
    std::queue <estructuraArbre> cua;
    cua.push(root);

    while(!cua.empty()){
        if(cua.front().equals(objectiu)) {
            return estructuraArbre(estructuraParella{0,0});
        }
        else if (cua.front().value().second == objectiu.value().second && 
                cua.front().right().empty() == objectiu.right().empty()) {
            return cua.front();
        }
        else if (! cua.front().right().empty()){
            cua.push(cua.front().right());
            cua.push(cua.front().left());
            cua.pop();
        }
        else {
            cua.pop();
        }
    }
    return estructuraArbre(estructuraParella{0,0});
}


estructuraArbre getLiderFullaSuperior(estructuraArbre root, estructuraArbre objectiu){
    std::queue <estructuraArbre> cua;
    cua.push(root);

    while(!cua.empty()){
        if(cua.front().equals(objectiu)) {
            return estructuraArbre(estructuraParella{0,0});
        }
        else if (cua.front().value().second == objectiu.value().second + 1 && 
                cua.front().right().empty()) {
            return cua.front();
        }
        else if (! cua.front().right().empty()){
            cua.push(cua.front().right());
            cua.push(cua.front().left());
            cua.pop();
        }
        else {
            cua.pop();
        }
    }
    return estructuraArbre(estructuraParella{0,0});
}


estructuraArbre getLiderInteriorIgual(estructuraArbre root, estructuraArbre objectiu){
    std::queue <estructuraArbre> cua;
    cua.push(root);

    while(!cua.empty()){
        if(cua.front().equals(objectiu)) {
            return estructuraArbre(estructuraParella{0,0});
        }
        else if (cua.front().value().second == objectiu.value().second && 
                ! cua.front().right().empty()) {
            return cua.front();
        }
        else if (! cua.front().right().empty()){
            cua.push(cua.front().right());
            cua.push(cua.front().left());
            cua.pop();
        }
        else {
            cua.pop();
        }
    }
    return estructuraArbre(estructuraParella{0,0});
}


void LliscaIncrementa(estructuraArbre root, estructuraArbre & node){
    estructuraArbre pareOrig = node.parent();

    if (node.right().empty()){
        //És fulla; lliscar fins el més alt dels interns de mateixa mida

        estructuraArbre lider = getLiderInteriorIgual(root, node);
        if (lider.value().second != 0) {
            lider.swap(node);
            node = lider;
            dirty = true;
        }

        estructuraParella temp = node.value();
        ++temp.second;
        node.setValue(temp);

        node = node.parent();
    }
    else{
        //És intern, lliscar fins la fulla més alta de mida n+1
        
        estructuraArbre lider = getLiderFullaSuperior(root, node);
        if (lider.value().second != 0) {
            lider.swap(node);
            node = lider;
            dirty = true;
        }

        estructuraParella temp = node.value();
        ++temp.second;
        node.setValue(temp);

        node = pareOrig;

    }
}


void recalculaCacheIntern(estructuraArbre arbre, estructuraCache & cauArbre, std::vector<bool> & cauNYT, std::vector<bool> cami){
    if (arbre.right().empty()){
        //fulla
        if (arbre.value().second == 0){
            //NYT
            cauNYT = cami;
        }
        else{
            //Node normal
            cauArbre[arbre.value().first] = cami;
        }
    }
    else{
        //intern
        cami.push_back(false);
        recalculaCacheIntern(arbre.left(), cauArbre, cauNYT, cami);
        cami.back() = true;
        recalculaCacheIntern(arbre.right(), cauArbre, cauNYT, cami);
        cami.pop_back();
    }
}


void recalculaCache(estructuraArbre root, estructuraCache & cauArbre, std::vector<bool> & cauNYT){
    cauArbre.clear();
    cauNYT.clear();
    recalculaCacheIntern(root, cauArbre, cauNYT, std::vector<bool>());
}


int main(int argc, char *argv[]){
    
    if (argc != 3){
        std::cout << "Us: " << argv[0] << " <input> <output>" << std::endl;
        exit(1);
    }

    estructuraArbre arbreAdaptatiu (estructuraParella {0,0});
    //Arbre sobre el qual crearem la codificació de Huffman adaptativa.
    //first: byte que codifica en cas de ser una fulla no NYT.
    //second: nombre d'iteracions del node. 0 per NYT.
    //S'inicia amb el node NYT.

    estructuraCache cacheArbre;
    //Mapa amb els bits que corresponen a cada byte per poder comprimir sense recòrrer l'arbre.

    std::vector<bool> cacheNYT;
    //Bits que corresponen a NTY. S'emmagatzema fora del map.

    estructuraArbre NYT (arbreAdaptatiu);
    //Punter a NYT.

    std::map<uint_least8_t, std::vector<bool>>::iterator posicio;
    //Iterador per trobarun byte a la caché de l'arbre.

    std::vector <bool> cacheByteActual;
    //Camí al node de l'ultim byte que hem llegit.

    FILE * entrada = fopen(argv[1], "r");
    __uint8_t bufferLectura[midaBuffer];
    int bytesLlegits;
    //Obrim fitxer a llegir, primer argument de la linia de comanda.
    //Variables per assistir a la lectura a baix nivell.

    EscriptorBits escriptor(argv[2]);
    //Objecte per escriure el fixter comprimit a disc.


    //Procès principal
    //
    //Llegim el fitxer d'entrada sencer

    estructuraArbre arbreTemporal, fullaIncrementar;

    while ((!std::feof(entrada)) && (bytesLlegits = fread(bufferLectura, sizeof(__uint8_t), midaBuffer, entrada))){
        for (int i = 0; i < bytesLlegits; ++i){
            dirty = false;

            //Mirem si hem enregistrat ja aquest byte abans 

            posicio = cacheArbre.find(bufferLectura[i]);

            arbreTemporal = estructuraArbre(arbreAdaptatiu);

            fullaIncrementar = estructuraArbre();

            if (posicio == cacheArbre.end()){

                //Si no el coneixem, escrivim a la sortida el camí a NYT seguit del byte nou a codificar.

                for (int j = 0; j < cacheNYT.size(); ++j) escriptor.escriuBit(cacheNYT[j]);
                escriptor.escriuBit(bufferLectura[i] & 0b10000000);
                escriptor.escriuBit(bufferLectura[i] & 0b01000000);
                escriptor.escriuBit(bufferLectura[i] & 0b00100000);
                escriptor.escriuBit(bufferLectura[i] & 0b00010000);
                escriptor.escriuBit(bufferLectura[i] & 0b00001000);
                escriptor.escriuBit(bufferLectura[i] & 0b00000100);
                escriptor.escriuBit(bufferLectura[i] & 0b00000010);
                escriptor.escriuBit(bufferLectura[i] & 0b00000001);

                //Obtenim el node a manipular

                for (int j = 0; j < cacheNYT.size(); ++j){
                    if (cacheNYT[j]) arbreTemporal = arbreTemporal.right();
                    else arbreTemporal = arbreTemporal.left();
                }
                
                arbreTemporal.addChildren(
                   estructuraArbre (estructuraParella {0,0}),
                   estructuraArbre (estructuraParella {bufferLectura[i],0})
                );
                fullaIncrementar = arbreTemporal.right();
                NYT = arbreTemporal.left();

                dirty = true;
            }
            else{

                //Si ja el coneixíem, escrivim a la sortida el camí al seu node.

                cacheByteActual = posicio->second;

                for (int j = 0; j < cacheByteActual.size(); ++j){
                    escriptor.escriuBit(cacheByteActual[j]);

                    //Obtenim el node a manipular

                    if (cacheByteActual[j]) arbreTemporal = arbreTemporal.right();
                    else arbreTemporal = arbreTemporal.left();
                }

                // Swap líder bloc
                estructuraArbre lider = getLiderBloc(arbreAdaptatiu, arbreTemporal);
                if (lider.value().second != 0) {
                    lider.swap(arbreTemporal);
                    arbreTemporal = lider;
                    dirty = true;
                }


                if (arbreTemporal.parent().left().equals(NYT)){
                    fullaIncrementar = arbreTemporal;
                    arbreTemporal = arbreTemporal.parent();
                }

            }


            while(! arbreTemporal.empty()){
                LliscaIncrementa(arbreAdaptatiu, arbreTemporal);
            }

            if (! fullaIncrementar.empty()){
                LliscaIncrementa(arbreAdaptatiu, fullaIncrementar);
            }

            if (dirty){
                recalculaCache(arbreAdaptatiu, cacheArbre, cacheNYT);
            }
        }
    }

    std::fclose(entrada);
    
    return 0;

}
