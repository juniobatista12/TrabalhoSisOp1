#include "../include/Tabela.h"
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SIZE_PATH 100

vector<vector<string>> historico;

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

int executa(vector<string> linhaComando){
    int pid = fork();
    int retorno;
    vector<char *> args;
    if (!pid){
        for (int i = 0; i < linhaComando.size(); i++){
            args.push_back(&linhaComando[i][0]);
        }
        args.push_back(nullptr);
        return execvp(args[0], args.data());
    }
    else{
        wait(NULL);
    }
}

void parse(vector<string> linhaComando, string inArquivo, string outArquivo){
    if (linhaComando[0] == "cd"){
        chdir(linhaComando[1].c_str());
    }
    else if (linhaComando[0] == "rm"){
        if(!remove(linhaComando[1].c_str())){
            cout << "Erro ao deletar arquivo" << endl;
        }
    }
    else if(linhaComando[0] == "historico"){
        if (linhaComando.size() != 1){
            parse(historico[stoi(linhaComando[1])], "", "");
        }
        else {
            try{
                for (int i = 0; i < historico.size(); i++){
                    cout << i << " ";
                    printVetor(historico[i]);
                }
            }
            catch(exception e){}
        }
    }
    else
        executa(linhaComando);
}

int main(){
    Tabela tabela = Tabela(".unbshrc");
    tabela.printTabela();
    string entrada;
    while (entrada != "exit"){
        vector<string> linhaComando;
        char cwd[100];
        getcwd(cwd,sizeof(cwd));
        cout << "UnBsh-user1-" << cwd<< ">";
        getline(cin, entrada);
        vector<string> tokens = split(entrada);
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
            for (int i = 0; tokens.size() && tokens[i] != "|" && tokens[i] != "<" && tokens[i] != ">";){
                linhaComando.push_back(tokens[i]);
                tokens.erase(tokens.begin());
            }
            string inArquivo = "", outArquivo = "";
            if (tokens[0] == "<"){
                tokens.erase(tokens.begin());
                inArquivo = tokens[0];
                tokens.erase(tokens.begin());
            }
            if (tokens[0] == ">"){
                tokens.erase(tokens.begin());
                outArquivo = tokens[0];
                tokens.erase(tokens.begin());
            }
            if (tokens[0] == "|" ){

            }
            parse(linhaComando, inArquivo, outArquivo);
        }
    }
}

// char *args[] = {"ls" "-hla", NULL}
// execv(args[0], args)