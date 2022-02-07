#ifndef TABELA_H
#define TABELA_H

#include <fstream>
#include <iostream>
#include <vector>
#include "LinhaTabela.h"

using namespace std;

class Tabela{
    vector<LinhaTabela> tabela;
public:
    Tabela(string);
    void printTabela();
};

#endif