#include "../include/Tabela.h"

vector <string> split(string linha){
    vector<string> tokens;
    string buffer;
    for (char letra : linha){
        if (letra == ' '){
            tokens.push_back(buffer);
            buffer.clear();
        }
        else{
            buffer += letra;
        }
    }
}

int main(){
    Tabela tabela = Tabela(".unbshrc");
    tabela.printTabela();
    string entrada;
    while (entrada != "exit"){
        cin >> entrada;
        vector<string> tokens = split(entrada);
    }
}