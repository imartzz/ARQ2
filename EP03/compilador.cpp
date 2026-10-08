/******************************************************************************
 * EP03 - ULA 4 bits + Arduino 
 *
 * Aluno: Arthur De Pinho De Almeida 888678
 *
 * Descrição:
 * Este programa lê um arquivo de código fonte no formato de mnemônicos (.ula),
 * realiza a tradução instrução por instrução mantendo o estado dos registradores
 * X e Y, trata erros de sintaxe e linhas em branco (reportando-os sem interromper
 * a carga), gera um arquivo de saída em formato hexadecimal (.hex) e exibe
 * a string concatenada pronta para ser enviada ao Arduino.
 *
 * Uso:
 *   ./compilador                        -> lê testeula.ula, gera testeula.hex
 *   ./compilador entrada.ula saida.hex  -> arquivos personalizados
 *
 * Mnemônicos suportados (Figura 2 do EP03 2026):
 *   nA     | (A+B)' -> AoBn  | A'.B   -> nAeB  | 0      -> zeroL
 *   (A.B)' -> AeBn  | B'    -> nB     | XOR    -> AxB    | A.B'   -> AenB
 *   A'+B   -> nAoB  | XNOR  -> AxBn   | B      -> copiaB | A.B    -> AeB
 *   1      -> umL   | A+B'  -> AonB   | A+B    -> AoB    | A      -> copiaA
 ******************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <stdexcept>

using namespace std;

// ─── Tabela de Mnemônicos 
// Mapeia cada mnemônico para o código hexadecimal correspondente (0x0 a 0xF)
// conforme a Figura 2
const map<string, char> OPCODES = {
    {"nA",     '0'}, // A'
    {"AoBn",   '1'}, // (A+B)'  - NOR
    {"nAeB",   '2'}, // A'.B
    {"zeroL",  '3'}, // 0       (0000 constante)
    {"AeBn",   '4'}, // (A.B)'  - NAND
    {"nB",     '5'}, // B'
    {"AxB",    '6'}, // XOR     - A'.B + A.B'
    {"AenB",   '7'}, // A.B'
    {"nAoB",   '8'}, // A' + B
    {"AxBn",   '9'}, // XNOR    - A.B + A'.B'
    {"copiaB", 'A'}, // B
    {"AeB",    'B'}, // A.B     - AND
    {"umL",    'C'}, // 1       (1111 constante)
    {"AonB",   'D'}, // A + B'
    {"AoB",    'E'}, // A + B   - OR
    {"copiaA", 'F'}, // A
};

// ─── Remove Espaços nas Extremidades da String 
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// ─── Converte Valor (0-15) para Caractere Hexadecimal 
char paraHexa(int val) {
    val &= 0x0F;
    return (val < 10) ? ('0' + val) : ('A' + val - 10);
}

// ─── Converte String Inteira para Inteiro (suporta decimal e hex 0x) 
int parseValor(const string& str) {
    if (str.empty()) throw invalid_argument("valor vazio");
    return stoi(str, nullptr, 0); // base 0 = autodetecta decimal / 0x hex
}

// ─── Processa uma Linha do Arquivo 
// Retorna:
//  true  = linha processada normalmente (ou ignorada propositalmente - inicio/fim)
//  false = erro detectado (linha em branco, sintaxe errada, mnemônico inválido)
bool processarLinha(const string& linhaOriginal, int numLinha,
                    int& valorX, int& valorY,
                    vector<string>& instrucoes) {
    string linha = trim(linhaOriginal);

    // ── Erro Tipo 1: Linha em branco 
    if (linha.empty()) {
        cout << " -> [ERRO  - Linha " << setw(3) << numLinha
             << "]: Linha em branco detectada (ignorada na carga)." << endl;
        return false;
    }

    // ── Marcadores de inicio e fim do programa 
    if (linha == "inicio:" || linha == "fim.") {
        cout << " -> [INFO  - Linha " << setw(3) << numLinha
             << "]: Marcador '" << linha << "' encontrado." << endl;
        return true;
    }

    // ── Remove ponto e vírgula no final (se houver) 
    if (linha.back() == ';') linha.pop_back();

    // ── Encontra o '=' separando variável do valor/mnemônico
    size_t posIgual = linha.find('=');
    if (posIgual == string::npos || posIgual == 0) {
        // Erro Tipo 2: Linha sem '=' - sintaxe completamente errada
        cout << " -> [ERRO  - Linha " << setw(3) << numLinha
             << "]: Sintaxe invalida (sem '='): '" << linha << "' (ignorada na carga)." << endl;
        return false;
    }

    string variavel = trim(linha.substr(0, posIgual));
    string valor    = trim(linha.substr(posIgual + 1));

    // Converte a variável para maiúsculas para aceitar x=, y=, w=
    string varUpper = variavel;
    transform(varUpper.begin(), varUpper.end(), varUpper.begin(), ::toupper);

    if (varUpper == "X") {
        // ── Atribuição de X
        try {
            valorX = parseValor(valor) & 0x0F; // Trunca para 4 bits
            cout << " -> [OK    - Linha " << setw(3) << numLinha
                 << "]: X = " << valorX << " (Hex: " << paraHexa(valorX) << ")" << endl;
        } catch (...) {
            cout << " -> [ERRO  - Linha " << setw(3) << numLinha
                 << "]: Valor invalido para X: '" << valor << "' (ignorado)." << endl;
            return false;
        }
    } else if (varUpper == "Y") {
        // ── Atribuição de Y 
        try {
            valorY = parseValor(valor) & 0x0F; // Trunca para 4 bits
            cout << " -> [OK    - Linha " << setw(3) << numLinha
                 << "]: Y = " << valorY << " (Hex: " << paraHexa(valorY) << ")" << endl;
        } catch (...) {
            cout << " -> [ERRO  - Linha " << setw(3) << numLinha
                 << "]: Valor invalido para Y: '" << valor << "' (ignorado)." << endl;
            return false;
        }
    } else if (varUpper == "W") {
        // ── Operação W = <mnemonico>
        auto it = OPCODES.find(valor);
        if (it != OPCODES.end()) {
            // Mnemônico válido: monta a instrução no formato XYS
            char hexX = paraHexa(valorX);
            char hexY = paraHexa(valorY);
            char hexS = it->second;
            string instrucao = "";
            instrucao += hexX;
            instrucao += hexY;
            instrucao += hexS;
            instrucoes.push_back(instrucao);
            cout << " -> [OK    - Linha " << setw(3) << numLinha
                 << "]: W = " << valor << " -> Instrucao gerada: " << instrucao << endl;
        } else {
            // Erro Tipo 2: Mnemônico com sintaxe errada
            cout << " -> [ERRO  - Linha " << setw(3) << numLinha
                 << "]: Mnemonico invalido/desconhecido: '" << valor << "' (ignorado na carga)." << endl;
            return false;
        }
    } else {
        // Erro Tipo 2: Variável desconhecida
        cout << " -> [ERRO  - Linha " << setw(3) << numLinha
             << "]: Variavel desconhecida '" << variavel << "' (ignorada na carga)." << endl;
        return false;
    }

    return true;
}

// ─── Ponto de Entrada 
int main(int argc, char* argv[]) {
    string arquivoEntrada = "testeula.ula";
    string arquivoSaida   = "testeula.hex";

    if (argc >= 2) arquivoEntrada = argv[1];
    if (argc >= 3) arquivoSaida   = argv[2];

    cout << "=====================================================" << endl;
    cout << "    COMPILADOR ULA 4 BITS  (.ula  ->  .hex)         " << endl;
    cout << "=====================================================" << endl;
    cout << "[+] Arquivo fonte : " << arquivoEntrada << endl;
    cout << "[+] Arquivo saida : " << arquivoSaida << endl;
    cout << "-----------------------------------------------------" << endl;

    // ── Abertura do arquivo de entrada 
    ifstream arqEntrada(arquivoEntrada);
    if (!arqEntrada.is_open()) {
        cerr << "[ERRO CRITICO] Nao foi possivel abrir '" << arquivoEntrada << "'!" << endl;
        return 1;
    }

    // ── Processamento linha a linha 
    vector<string> instrucoes; // Lista de instruções hexadecimais geradas
    int valorX = 0;            // Estado atual do registrador X
    int valorY = 0;            // Estado atual do registrador Y
    int numLinha = 0;
    int totalErros = 0;
    string linha;

    while (getline(arqEntrada, linha)) {
        numLinha++;
        bool ok = processarLinha(linha, numLinha, valorX, valorY, instrucoes);
        if (!ok) totalErros++;
    }
    arqEntrada.close();

    // ── Geração do arquivo de saída (.hex) 
    ofstream arqSaida(arquivoSaida);
    if (!arqSaida.is_open()) {
        cerr << "ERRO Nao foi possivel criar '" << arquivoSaida << "'!" << endl;
        return 1;
    }

    // Cada instrução ocupa uma linha no arquivo .hex
    string hexConcatenado = ""; // String única para copiar e colar no Tinkercad
    for (const string& instr : instrucoes) {
        arqSaida << instr << "\n";
        hexConcatenado += instr;
    }
    arqSaida.close();

    // ── Resumo do Processamento
    cout << "=====================================================" << endl;
    cout << "[+] Resumo da compilacao:" << endl;
    cout << "    - Linhas lidas            : " << numLinha << endl;
    cout << "    - Erros detectados        : " << totalErros << " (ignorados na carga)" << endl;
    cout << "    - Instrucoes validas geradas: " << instrucoes.size() << endl;
    cout << "=====================================================" << endl;
    cout << "\nCODIGO HEX PARA COPIAR NO TINKERCAD/ARDUINO:" << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << hexConcatenado << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "[+] Arquivo '" << arquivoSaida << "' gerado com sucesso!" << endl;

    return 0;
}
