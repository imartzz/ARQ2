# GUIA COMPLETO - EXERCÍCIO PRÁTICO 03 (EP03)
**Disciplina:** Arquitetura de Computadores II  
**Aluno:** Arthur De Pinho De Almeida (Matrícula: 888678)  

---

## 📁 Estrutura de Arquivos

* `compilador.cpp`: Código em C++ para a interface no PC. Lê o arquivo `.ula`, detecta erros, gera o arquivo `.hex` e exibe o código concatenado para o Arduino.
* `arduino_ula.ino`: Código do Arduino (para colar no **Tinkercad** ou IDE do Arduino). Implementa a ULA de 4 bits, vetor de memória de 100 posições, ciclo de busca/execução, exibição do DUMP e acionamento dos 4 LEDs.
* `testeula.ula`: Arquivo de teste fonte contendo mnemônicos, atribuições de variáveis, linhas em branco e erros de sintaxe propositais.
* `testeula.hex`: Arquivo resultante da compilação com os códigos hexadecimais gerados.

---

## 🛠️ 1. Como Compilar e Rodar o Programa C++ no Linux

No terminal, navegue até a pasta do projeto e execute:

```bash
g++ -std=c++17 compilador.cpp -o compilador
./compilador testeula.ula testeula.hex
```

O programa exibirá no terminal:
1. Leitura linha a linha do arquivo `.ula`.
2. Alertas detalhados de erros sintáticos e linhas em branco detectadas.
3. Resumo com o número de erros e instruções geradas.
4. A **String Hexadecimal Completa** pronta para ser copiada para o Tinkercad/Arduino.

---

## ⚡ 2. Como Montar e Simular no Tinkercad

### 🔌 Montagem do Hardware no Tinkercad:
1. Adicione um **Arduino Uno**.
2. Adicione **4 LEDs** (conecte os anodos nos pinos digitais do Arduino e os catodos ao GND com resistores de 220Ω):
   * **Pino 13:** LED F3 (Bit 3 - Bit Mais Significativo / MSB)
   * **Pino 12:** LED F2 (Bit 2)
   * **Pino 11:** LED F1 (Bit 1)
   * **Pino 10:** LED F0 (Bit 0 - Bit Menos Significativo / LSB)

### 💻 Carregando o Código no Tinkercad:
1. Abra a aba **Código** no Tinkercad e mude para o modo **Texto**.
2. Cole todo o conteúdo do arquivo `arduino_ula.ino`.
3. Clique em **Iniciar Simulação**.
4. Abra o **Monitor Serial** (no rodapé da tela do Tinkercad).

---

## 🚀 3. Passo a Passo da Execução na Apresentação

1. **Carga Inicial:**
   * Copie a linha de caracteres hexadecimais gerada pelo compilador C++ (exemplo: `C6BA3EA34D35D33D3CD3FD3AD36`).
   * Cole a linha inteira no campo de envio do Monitor Serial do Tinkercad e pressione **Enviar**.
   * O Arduino mostrará a mensagem de confirmação e o **DUMP de Carga do Vetor**:
     ```text
     - >| 4 | 0 | 0 | 0 | C6B | A3E | A34 | D35 | ... |
     ```

2. **Confirmação:**
   * O Arduino perguntará: `Quer executar o programa? (s/n)`
   * Digite `s` (ou `sim`) no campo de envio e pressione **Enviar**.

3. **Acompanhamento (Ciclo de Execução):**
   * A cada **4 segundos**, o Arduino lerá a instrução indicada pelo **PC**, atualizará os registradores `X`, `Y` e `W`, acenderá os **LEDs** referentes ao valor de `W`, e imprimirá uma nova linha de **DUMP de Memória**.

---

## 📋 Tabela de Mnemônicos e Operações da ULA

| Hex | Mnemônico | Função Lógica | Operação |
|:---:|:---------:|:-------------:|:--------:|
| 0 | `nA` | A' | NOT A |
| 1 | `AoBn` | (A+B)' | NOR(A, B) |
| 2 | `nAeB` | A'.B | NOT(A) AND B |
| 3 | `zeroL` | 0 | Nulo (0000) |
| 4 | `AeBn` | (A.B)' | NAND(A, B) |
| 5 | `nB` | B' | NOT B |
| 6 | `AxB` | A'.B + A.B' | XOR(A, B) |
| 7 | `AenB` | A.B' | A AND NOT(B) |
| 8 | `nAoB` | A' + B | NOT(A) OR B |
| 9 | `AxBn` | A.B + A'.B' | XNOR(A, B) |
| A | `copiaB` | B | Passa B |
| B | `AeB` | A.B | A AND B |
| C | `umL` | 1 (1111) | Nível Lógico 1 (1111) |
| D | `AonB` | A + B' | A OR NOT(B) |
| E | `AoB` | A + B | A OR B |
| F | `copiaA` | A | Passa A |
