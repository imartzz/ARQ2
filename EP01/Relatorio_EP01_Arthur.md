# Relatório EP01 - Prática de Arquitetura de Computadores

**Aluno:** Arthur [Seu Sobrenome]
**Matrícula:** [Sua Matrícula - Início 88, Fim 78]

---

## Parte 1 - Somadores (Logisim)

**1.1 Circuito do Meio-Somador:**
*(Insira a imagem `circuito_logisim_meio_somador.png` aqui)*

**1.2 Circuito do Somador Completo de 1 bit:**
*(Insira a imagem `circuito_logisim_somador_completo_1bit.png` aqui)*

**Soma da matrícula (Início $8+8=16$) no Somador de 4 bits:**
*(Insira a imagem `circuito_logisim_matricula_inicio_8_8.png` aqui)*

**Respostas Teóricas - Parte 1:**
**Por que dizemos que os bits mais significativos têm um atraso na reposta e que o somador sofre com o efeito de propagação?**
Dizemos que há atraso e efeito de propagação (*ripple carry*) porque cada Somador Completo de 1 bit precisa esperar o cálculo do "vai-um" ($C_{out}$) do bit anterior ($C_{in}$) para conseguir concluir a sua própria soma. Como os somadores estão ligados em cascata, o bit mais significativo (o último da esquerda) é o que mais demora a ficar pronto, pois ele depende que o carry se propague fisicamente por todas as portas lógicas dos bits anteriores.

---

## Parte 2 - Decodificadores e Displays (Logisim)

**Soma da matrícula (Fim $7+8=F$) com Displays:**
*(Insira a imagem `circuito_logisim_matricula_fim_7_8_F.png` aqui)*

*(Nota: Este print demonstra o uso dos decodificadores de 7 segmentos convertendo os sinais binários para exibição em hexadecimal nos displays do circuito `main`).*

---

## Parte 3 - Calculadora de 4 bits (Logisim)

Nesta etapa, o somador foi utilizado para realizar subtrações utilizando a notação de Complemento de 2.

**a) +5 + (-6):**
*(Insira a imagem `circuito_logisim_parte3_a_5_mais_menos6.png` aqui)*
**Displays exibem:** `5`, `A`, `F` (resultado = -1).

**b) -5 + (-5):**
*(Insira a imagem `circuito_logisim_parte3_b_menos5_mais_menos5.png` aqui)*
**Displays exibem:** `b`, `b`, `6` (resultado com overflow visualizado no display).

**c) -7 + (-9):**
*(Insira a imagem `circuito_logisim_parte3_c_menos7_mais_menos9.png` aqui)*
**Displays exibem:** `9`, `7`, `0`. 
*(Nota: -9 truncado em 4 bits vira +7, gerando a soma -7 + 7 = 0).*

**d) +7 + 9:**
*(Insira a imagem `circuito_logisim_parte3_d_mais7_mais_mais9.png` aqui)*
**Displays exibem:** `7`, `9`, `0`.
*(Nota: +9 truncado em 4 bits vira -7, gerando a soma 7 + (-7) = 0).*

**Respostas Teóricas - Parte 3:**

**1) Por que as duas últimas operações irão ligar o overflow?**
Matematicamente, as somas $-7 + (-9) = -16$ e $+7 + 9 = +16$ resultam em valores que estão fora do intervalo representável em 4 bits com sinal ($-8$ a $+7$), o que por definição caracteriza *overflow*. Porém, como os números $\pm9$ não possuem representação válida em 4 bits (o -9 vira `0111`=+7, e o +9 vira `1001`=-7), ao inserirmos essas chaves no circuito, o somador acaba processando cálculos de sinais opostos ($-7 + 7 = 0$), resultando em $Co_3 \oplus Co_2 = 0$. Portanto, o circuito físico se comporta corretamente e **não acende** o LED de overflow nessas operações inválidas.

**2) Qual a porta lógica que você utilizou para esta operação de identificar o overflow?**
Foi utilizada uma porta **XOR** (Ou-Exclusivo) comparando o Carry-out do penúltimo estágio ($Co_2$) com o Carry-out do último estágio ($Co_3$). A regra matemática implementada é: $Overflow = Co_3 \oplus Co_2$.

---

## Parte 4 - Flags (Zero e Negativo)

**Implementação Interna dos Flags no Somador de 4 bits:**
*(Insira a imagem `circuito_logisim_parte4_flags.png` aqui)*

**Respostas Teóricas - Parte 4:**

**Você consegue pensar em alguma situação onde esses flags possam ser utilizados?**
Os flags são componentes fundamentais na ULA (Unidade Lógica e Aritmética) de qualquer processador, pois eles alimentam o registrador de estado (*Status Register*). Eles são amplamente utilizados para o controle de fluxo do programa através de **instruções de desvio condicional** (saltos como `if/else`). 
Por exemplo: ao comparar duas variáveis com uma subtração ($A - B$), se o **Flag Zero** acender, o processador sabe que o resultado foi zero e conclui que $A = B$ (ativando instruções de branch como o `BEQ`). Da mesma forma, o **Flag Negativo** ajuda a identificar se $A < B$ (ativando instruções como o `BLT`), permitindo tomadas de decisão na execução do código.

---

## Parte 5 e 6 - Tinkercad (Hardware Físico e C++)

**Circuito Físico com CIs (74HC86, 74HC08, 74HC32):**
*(Insira a imagem `circuito_tinkercad_somador_1bit.png` aqui)*

**Circuito Programado no Arduino Uno R3 (C++):**
*(Insira a imagem `circuito_arduino_somador_1bit.png` aqui)*
