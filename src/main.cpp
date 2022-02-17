#include "../include/Tabela.h"
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SIZE_PATH 100

vector<vector<string>> historico;
vector<string> path;
Tabela tabela = Tabela(".unbshrc");

string adicionaBarra(char *cwd);
vector <string> split(string linha, char caractere);
void printVetor(vector<char *> tokens);
void printVetor(vector<string> tokens);
int executa(vector<string> linhaComando, string inArquivo, string outArquivo);
void trataBuiltin(vector<string> linhaComando, string inArquivo, string outArquivo);
void parse(vector<string> tokens);

int main(int argc, char *argv[]){
    ifstream fin;
    string tmp;
    fin.open(".unbshrc_profile");
    fin >> tmp;
    fin.close();
    path = split(tmp, ';');
    path[0].erase(path[0].begin(), path[0].begin() + 5);
    printVetor(path);
    tabela.printTabela();
    string entrada;
    char cwd[100];
    if (argc == 1){
        while (entrada != "exit" && entrada != "saida"){
            getcwd(cwd,sizeof(cwd));
            cout << "UnBsh-user1-" << cwd<< ">";
            getline(cin, entrada);
            vector<string> tokens = split(entrada, ' ');
            path.push_back(adicionaBarra(cwd));
            parse(tokens);
            path.pop_back();
        }
    }
    else{
        fin.open(argv[1]);
        while (getline(fin, entrada)){
            if (entrada[0] == '#'){
                continue;
            }
            vector<string> tokens = split(entrada, ' ');
            getcwd(cwd, sizeof(cwd));
            path.push_back(adicionaBarra(cwd));
            parse(tokens);
            path.pop_back();
        }
    }
}

string adicionaBarra(char *cwd){
    int i = 0;
    string tmp;
    while (cwd[i]){
        tmp += cwd[i++];
    }
    tmp += "/";
    return tmp;
}

vector <string> split(string linha, char caractere){
    vector<string> tokens;
    string buffer;
    for (char letra : linha){
        if (letra == caractere && buffer != ""){
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
vector<char *> converteVetor(vector<string> entrada){
    vector <char *> args;
    
    return args;
}

int executa(vector<string> linhaComando, string inArquivo, string outArquivo, bool appArquivo){
    int pid = fork();
    int retorno;
    bool background = linhaComando[linhaComando.size() - 1] == "&";
    if (background){
        linhaComando.pop_back();
    }
    if (!pid){
        if (inArquivo != ""){
            int fin = open(inArquivo.c_str(), O_RDONLY);
            dup2(fin, 0);
        }
        if (outArquivo != ""){
            int mask = appArquivo ? O_APPEND : 0;
            int fout = open(outArquivo.c_str(), O_CREAT | mask |  O_WRONLY, S_IRWXU | S_IRWXG);
            dup2(fout, 1);
        }
        for (string caminho : path){          
            vector<string> copia = linhaComando;
            copia[0] = caminho + copia[0];
            vector<char *> args = converteVetor(copia);
            for (int i = 0; i < copia.size(); i++){
                cout << copia[i] << "\t";
                args.push_back(&*copia[i].begin());
            }
            args.push_back(nullptr);
            execv(args[0], args.data());
        }
        cout << "Comando nao encontrado" << endl;
    }
    else{
        if (background){
            int status;
            cout << "Processo em background [" << pid << "] foi iniciado" << endl;
            waitpid(-1, &status, WNOHANG);
        }
        else {
            wait(NULL);
        }
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
    else if (linhaComando[0] == "ver"){
        cout << "v1.0" << endl << "Ultima atualizacao: 10/02/2022" << endl << "Autor: Divino Junio Batista Lopes" << endl;
    }
    else if (linhaComando[0] == "exit" || linhaComando[0] == "sair"){}
    else
        executa(linhaComando, inArquivo, outArquivo, appArquivo);
}

void parse(vector<string> tokens){
    bool piped = false;
    int i = 0;
    historico.push_back(tokens);
    if (historico.size() > 10){
        historico.erase(historico.begin());
    }
    while (tokens.size()){
        vector<string> linhaComando;
        for (LinhaTabela linha : tabela.tabela){
            if(tokens[0] == linha.alias){
                tokens[0] = linha.cmd;
            }
        }
        for (int i = 0; tokens.size() && tokens[i] != "|" && tokens[i] != "<" && tokens[i] != ">" && tokens[i] != ">>";){
            linhaComando.push_back(tokens[i]);
            tokens.erase(tokens.begin());
        }
        string inArquivo = "", outArquivo = "";
        bool appArquivo = false;
        if (piped){
            inArquivo = ".tmp" + to_string(i);
            i ^= 1;
            piped = false;
        }
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
        else if (tokens[0] == "|"){
            tokens.erase(tokens.begin());
            outArquivo = ".tmp" + to_string(i);
            piped = true;
        }
        trataBuiltin(linhaComando, inArquivo, outArquivo, appArquivo);
    }
    remove(".tmp0");
    remove(".tmp1");
}
// char *args[] = {"ls" "-hla", NULL}
// execv(args[0], args)