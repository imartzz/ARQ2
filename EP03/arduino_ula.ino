/******************************************************************************
 * EP03 - ULA 4 bits + Arduino (Código para o Hardware/Tinkercad)
 *
 * Aluno: Arthur De Pinho De Almeida (Matrícula: 888678)
 * Disciplina: Arquitetura de Computadores II
 *
 * Descrição:
 * Este programa simula uma ULA de 4 bits no Arduino com memória interna de 100 posições.
 *
 * Estrutura da Memória (vetor String de 100 posições):
 *   - memoria[0]: PC (Program Counter) - índice da instrução a ser executada, inicia em 4
 *   - memoria[1]: W  (resultado da última operação executada)
 *   - memoria[2]: X  (operando A / variável X)
 *   - memoria[3]: Y  (operando B / variável Y)
 *   - memoria[4..99]: Área de instruções (programa carregado)
 *
 * Pinos de saída dos LEDs (resultado W da ULA):
 *   - Pino 13: F3 (Bit 3 - MSB)
 *   - Pino 12: F2 (Bit 2)
 *   - Pino 11: F1 (Bit 1)
 *   - Pino 10: F0 (Bit 0 - LSB)
 *
 * Tabela de Instruções da ULA (Figura 2 do EP03 2026):
 *   0=nA (A')       1=AoBn ((A+B)')    2=nAeB (A'.B)    3=zeroL (0)
 *   4=AeBn ((A.B)') 5=nB (B')          6=AxB (XOR)      7=AenB (A.B')
 *   8=nAoB (A'+B)   9=AxBn (XNOR)      A=copiaB (B)     B=AeB (A.B)
 *   C=umL (1)       D=AonB (A+B')      E=AoB (A+B)      F=copiaA (A)
 ******************************************************************************/

// ─── Memória da Máquina ───────────────────────────────────────────────────────
String memoria[100]; // Vetor de memória: posições 0-3 são registradores, 4-99 são instruções

// ─── Variáveis de Controle ────────────────────────────────────────────────────
int pc = 4;           // Program Counter - aponta para a instrução atual (inicia em 4)
int n  = 0;           // Número de instruções carregadas no vetor
int pos_input = 0;    // Posição atual de leitura dentro da string de entrada (serial)
char W;               // Caractere hexadecimal do resultado da operação atual
char regX;            // Último operando X usado (para display direto)
char regY;            // Último operando Y usado (para display direto)

// ─── Definição dos Pinos dos LEDs ────────────────────────────────────────────
const int PINO_F3 = 13; // Bit 3 (MSB)
const int PINO_F2 = 12; // Bit 2
const int PINO_F1 = 11; // Bit 1
const int PINO_F0 = 10; // Bit 0 (LSB)

// ─── Inicialização ────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial a 9600 bps
  
  // Configura os pinos dos LEDs como saída
  pinMode(PINO_F3, OUTPUT);
  pinMode(PINO_F2, OUTPUT);
  pinMode(PINO_F1, OUTPUT);
  pinMode(PINO_F0, OUTPUT);

  Serial.println("=================================================");
  Serial.println("         SIMULADOR ULA 4 BITS - ARDUINO          ");
  Serial.println("=================================================");
  Serial.println("Cole a string hexadecimal gerada pelo compilador:");
  Serial.println("(ex: C6BA3EA34D35)");
}

// ─── Loop Principal ──────────────────────────────────────────────────────────
void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readString();
    input.trim(); // Remove espaços e quebras de linha na entrada
    
    if (input.length() == 0) return;

    // Carrega as instruções na memória e executa
    load_memory(input);
    program_execution();

    // Reseta para uma nova carga
    pc = 4;
    n = 0;
    pos_input = 0;
    Serial.println("\nPronto para nova carga. Cole a proxima string:");
  }
}

// ─── Carrega as Instruções no Vetor Memória ──────────────────────────────────
// Recebe a string hexadecimal concatenada (ex: "C6BA3EA34")
// e divide em triplas, preenchendo a memória a partir do índice 4.
void load_memory(String input) {
  // Reseta a área de memória para evitar lixo de cargas anteriores
  for (int i = 0; i < 100; i++) {
    memoria[i] = "";
  }

  // Inicializa os registradores
  pc = 4;
  memoria[0] = String(pc); // PC inicial aponta para o índice 4
  memoria[1] = "0";        // W começa em 0
  memoria[2] = "0";        // X começa em 0
  memoria[3] = "0";        // Y começa em 0
  n = 0;
  pos_input = 0;

  // Lê a string de entrada em blocos de 3 caracteres hexadecimais (XYS)
  while (pos_input < input.length() && n + 4 < 100) {
    String sub = get_sub_string(input);
    if (sub.length() < 3) break; // Interrompe se não houver uma tripla completa de 3 caracteres
    memoria[n + 4] = sub;
    n++;
  }

  Serial.println("\n[+] Carga do vetor realizada!");
  display_memoria(); // Exibe o DUMP inicial com a carga do vetor
}

// ─── Extrai uma Tripla de 3 Caracteres da String de Entrada ──────────────────
// Ignora espaços e '\n' durante a leitura (tolerante a formatações)
String get_sub_string(String input) {
  int j = 0;
  String sub = "";
  while (j < 3 && pos_input < input.length()) {
    char c = input.charAt(pos_input);
    if (c != ' ' && c != '\n' && c != '\r') {
      sub += c;
      j++;
    }
    pos_input++;
  }
  return sub;
}

// ─── Ciclo de Execução do Programa ────────────────────────────────────────────
// Exibe o DUMP inicial, pergunta confirmação e executa instrução por instrução.
void program_execution() {
  Serial.println("\nQuer executar o programa? (s/n)");

  // Aguarda a resposta do usuário (s ou n)
  while (!Serial.available()) { delay(100); }
  String resp = Serial.readString();
  resp.trim();

  if (!resp.equalsIgnoreCase("s") && !resp.equalsIgnoreCase("sim")) {
    Serial.println("[!] Execução cancelada.");
    return;
  }

  Serial.println("\nIniciando execucao...\n");

  // Loop principal de busca e execução de instruções
  for (int i = 0; i < n; i++) {
    String instrucao = memoria[pc]; // Busca a instrução na posição apontada pelo PC
    
    if (instrucao.length() < 3) {
      break; // Interrompe a execução ao encontrar instrução incompleta/vazia
    }

    char charX = instrucao.charAt(0); // 1º nibble hexadecimal = operando X
    char charY = instrucao.charAt(1); // 2º nibble hexadecimal = operando Y
    char charS = instrucao.charAt(2); // 3º nibble hexadecimal = seletor de operação

    // Executa a operação da ULA e obtém o resultado numérico
    int resultadoInt = executar_ula(charX, charY, charS);

    // Converte o resultado inteiro (0-15) para caractere hexadecimal ('0'-'F')
    W = converter_para_hexa(resultadoInt);

    // Salva operandos para display
    regX = charX;
    regY = charY;

    // Aciona os LEDs com os bits do resultado
    acionar_leds(resultadoInt);

    // Exibe o DUMP da memória com seta indicando instrução executada
    display_memoria_com_pc(pc);

    // Incrementa o PC para a próxima instrução
    pc++;

    // Intervalo de 4 segundos entre instruções (conforme especificação do EP)
    delay(4000);
  }

  Serial.println("\n=== Execucao finalizada! ===\n");
}

// ─── Executa a Operação Lógica da ULA ────────────────────────────────────────
// Recebe os caracteres hexadecimais x, y e s, converte para inteiros e executa.
// Retorna o resultado inteiro no intervalo 0-15 (4 bits).
int executar_ula(char x, char y, char s) {
  int A = converter_para_int(x); // Operando A (variável X)
  int B = converter_para_int(y); // Operando B (variável Y)
  int resultado = 0;

  // Conjunto de instruções conforme Figura 2 do EP03 2026 (ativos em 1 - high)
  switch (s) {
    case '0': resultado = (~A);       break; // nA     : A'
    case '1': resultado = ~(A | B);   break; // AoBn   : (A+B)' - NOR
    case '2': resultado = (~A) & B;   break; // nAeB   : A'.B
    case '3': resultado = 0;          break; // zeroL  : 0 (0000)
    case '4': resultado = ~(A & B);   break; // AeBn   : (A.B)' - NAND
    case '5': resultado = (~B);       break; // nB     : B'
    case '6': resultado = A ^ B;      break; // AxB    : XOR
    case '7': resultado = A & (~B);   break; // AenB   : A.B'
    case '8': resultado = (~A) | B;   break; // nAoB   : A' + B
    case '9': resultado = ~(A ^ B);   break; // AxBn   : XNOR
    case 'A': resultado = B;          break; // copiaB : B
    case 'B': resultado = A & B;      break; // AeB    : A.B - AND
    case 'C': resultado = 0x0F;       break; // umL    : 1 (1111)
    case 'D': resultado = A | (~B);   break; // AonB   : A + B'
    case 'E': resultado = A | B;      break; // AoB    : A + B - OR
    case 'F': resultado = A;          break; // copiaA : A
    default:
      Serial.println("[ERRO] Operacao invalida: " + String(s));
      resultado = 0;
  }

  return resultado & 0x0F; // Garante que o resultado está em 4 bits (0 a 15)
}

// ─── Aciona os LEDs com os Bits do Resultado ─────────────────────────────────
// valorW deve estar no intervalo 0-15 (4 bits).
void acionar_leds(int valorW) {
  valorW &= 0x0F;
  digitalWrite(PINO_F3, (valorW & 0x08) ? HIGH : LOW); // Bit 3 (MSB)
  digitalWrite(PINO_F2, (valorW & 0x04) ? HIGH : LOW); // Bit 2
  digitalWrite(PINO_F1, (valorW & 0x02) ? HIGH : LOW); // Bit 1
  digitalWrite(PINO_F0, (valorW & 0x01) ? HIGH : LOW); // Bit 0 (LSB)
}

// ─── Exibe DUMP da Memória (Carga Inicial) ───────────────────────────────────
// Usa Serial.print() direto para evitar alocação de heap (SRAM limitada no Uno)
void display_memoria() {
  Serial.print("- >| ");
  Serial.print(memoria[0]); Serial.print(" | ");
  Serial.print(memoria[1]); Serial.print(" | ");
  Serial.print(memoria[2]); Serial.print(" | ");
  Serial.print(memoria[3]); Serial.print(" | ");
  for (int i = 4; i < n + 4; i++) {
    Serial.print(memoria[i]);
    Serial.print(" | ");
  }
  Serial.println();
}

// ─── Exibe DUMP da Memória com Seta no PC Atual ──────────────────────────────
// Usa Serial.print() direto — cada chamada envia bytes imediatamente ao buffer
// serial sem alocar memória no heap, evitando crash silencioso por falta de SRAM.
void display_memoria_com_pc(int address) {
  Serial.print(" | ");
  Serial.print(pc);
  Serial.print(" | ");
  Serial.print(W);
  Serial.print(" | ");
  Serial.print(regX);
  Serial.print(" | ");
  Serial.print(regY);
  Serial.print(" | ");
  for (int i = 4; i < n + 4; i++) {
    if (i == address) {
      Serial.print("->");
    }
    Serial.print(memoria[i]);
    Serial.print(" | ");
  }
  Serial.println();
}

// ─── Converte Caractere Hexadecimal para Inteiro ─────────────────────────────
// '0'-'9' -> 0-9,  'A'-'F' ou 'a'-'f' -> 10-15
int converter_para_int(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
  if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
  return 0;
}

// ─── Converte Inteiro (0-15) para Caractere Hexadecimal ──────────────────────
// 0-9 -> '0'-'9',  10-15 -> 'A'-'F'
char converter_para_hexa(int x) {
  x &= 0x0F;
  if (x >= 0 && x <= 9)  return '0' + x;
  return 'A' + (x - 10);
}
