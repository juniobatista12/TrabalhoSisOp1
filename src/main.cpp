#include "../include/Tabela.h"
#include <fcntl.h>
#include <fstream>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SIZE_PATH 100

vector<vector<string>> historico;
Tabela tabela = Tabela(".unbshrc");

vector <string> split(string linha);
void printVetor(vector<char *> tokens);
void printVetor(vector<string> tokens);
int executa(vector<string> linhaComando, string inArquivo, string outArquivo);
void trataBuiltin(vector<string> linhaComando, string inArquivo, string outArquivo);
void parse(vector<string> tokens);

int main(int argc, char *argv[]){
    tabela.printTabela();
    string entrada;
    if (argc == 1){
        while (entrada != "exit" && entrada != "saida"){
            char cwd[100];
            getcwd(cwd,sizeof(cwd));
            cout << "UnBsh-user1-" << cwd<< ">";
            getline(cin, entrada);
            vector<string> tokens = split(entrada);
            parse(tokens);
        }
    }
    else{
        ifstream fin;
        fin.open(argv[1]);
        while (getline(fin, entrada)){
            if (entrada[0] == '#'){
                continue;
            }
            vector<string> tokens = split(entrada);
            parse(tokens);
        }
    }
}

vector <string> split(string linha){
    vector<string> tokens;
    string buffer;
    for (char letra : linha){
        if (letra == ' ' && buffer != ""){
            tokens.push_back(buffer);
            buffer = "";
        }
        else{
            buffer += letra;
        }
    }
    tokens.push_back(buffer);
    return tokens;
}

void printVetor(vector<char *> tokens){
    for (auto token : tokens){
        cout << token << " ";
    }
    cout << endl;
}

void printVetor(vector<string> tokens){
    for (auto token : tokens){
        cout << token << " ";
    }
    cout << endl;
}

int executa(vector<string> linhaComando, string inArquivo, string outArquivo, bool appArquivo){
    int pid = fork();
    int retorno;
    vector<char *> args;
    if (!pid){
        for (int i = 0; i < linhaComando.size(); i++){
            args.push_back(&linhaComando[i][0]);
        }
        args.push_back(nullptr);
        if (inArquivo != ""){
            cout << "Entrei aqui" << endl;
            int fin = open(inArquivo.c_str(), O_RDONLY);
            dup2(fin, 0);
        }
        if (outArquivo != ""){
            int mask = appArquivo ? O_APPEND : 0;
            int fout = open(outArquivo.c_str(), O_CREAT | mask |  O_WRONLY, S_IRWXU | S_IRWXG);
            dup2(fout, 1);
        }
        execvp(args[0], args.data());
    }
    else{
        wait(NULL);
    }
}

void trataBuiltin(vector<string> linhaComando, string inArquivo, string outArquivo, bool appArquivo){
    if (linhaComando[0] == "cd"){
        chdir(linhaComando[1].c_str());
    }
    else if (linhaComando[0] == "rm"){
        if(remove(linhaComando[1].c_str()) == -1){
            cout << "Erro ao deletar arquivo" << endl;
        }
    }
    else if(linhaComando[0] == "historico"){
        if (linhaComando.size() != 1){
            parse(historico[stoi(linhaComando[1])]);
        }
        else {
            for (int i = 0; i < historico.size(); i++){
                cout << i << " ";
                printVetor(historico[i]);
            }
        }
    }
    else
        executa(linhaComando, inArquivo, outArquivo, appArquivo);
}

void parse(vector<string> tokens){
    vector<string> linhaComando;
    while (tokens.size()){
        for (LinhaTabela linha : tabela.tabela){
            if(tokens[0] == linha.alias){
                tokens[0] = linha.cmd;
            }
        }
        historico.push_back(tokens);
        if (historico.size() > 10){
            historico.erase(historico.begin());
        }
        for (int i = 0; tokens.size() && tokens[i] != "|" && tokens[i] != "<" && tokens[i] != ">" && tokens[i] != ">>";){
            linhaComando.push_back(tokens[i]);
            tokens.erase(tokens.begin());
        }
        string inArquivo = "", outArquivo = "";
        bool appArquivo = false;
        if (tokens[0] == "<"){
            tokens.erase(tokens.begin());
            inArquivo = tokens[0];
            tokens.erase(tokens.begin());
        }
        else if (tokens[0] == ">"){
            tokens.erase(tokens.begin());
            outArquivo = tokens[0];
            tokens.erase(tokens.begin());
        }
        else if(tokens[0] == ">>"){
            tokens.erase(tokens.begin());
            outArquivo = tokens[0];
            appArquivo = true;
            tokens.erase(tokens.begin());
        }
        else if (tokens[0] == "|" ){

        }
        trataBuiltin(linhaComando, inArquivo, outArquivo, appArquivo);
    }
}
// char *args[] = {"ls" "-hla", NULL}
// execv(args[0], args)