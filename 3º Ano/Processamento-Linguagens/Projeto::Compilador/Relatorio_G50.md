# Projeto de Processamento de Linguagens

## Construção de um Compilador para Pascal Standard

#### Trabalho realizado por:
Grupo 50

a104270 - João Pedro Loureiro Pinto
a104183 - Diogo Alexandre Gomes Silva
a97941 - Diogo Filipe Oliveira da Silva


## Indice

1. [Introdução](#1-introdução)
2. [Arquitetura do Sistema](#2-arquitetura-do-sistema)
3. [Analisador Léxico](#3-analisador-léxico)
    3.1. [Estrutura](#31-estrutura)
    3.2. [Como funciona?](#32-como-funciona)
4. [Analisador Sintático](#4-analisador-sintático)
    4.1. [Estrutura](#41-estrutura)
    4.2. [Gramática](#42-gramática)
    4.3. [Como funciona?](#43-como-funciona)
5. [Analisador Semântico](#5-analisador-semântico)
    5.1. [Estrutura](#51-estrutura)
    5.2. [Como funciona?](#52-como-funciona)
6. [Geração de Código para a Máquina Virtual](#6-geração-de-código-para-a-máquina-virtual)
    6.1. [Input/Output](#61-inputoutput)
    6.2. [Condições](#62-condições)
    6.3. [Ciclos](#63-ciclos)
7. [Fluxo de execução](#7-fluxo-de-execução)
8. [Como Executar](#8-como-executar)
9. [Conclusão](#9-conclusão) 

## 1. Introdução
Este projeto consiste no desenvolvimento de um compilador para a linguagem Pascal Standard. O objetivo principal do projeto consistiu na implementação de um compilador funcional que fosse capaz de interpretar, validar e traduzir código Pascal para um conjunto de instruções para a máquina virtual (VM). 
O compilador foi desenvolvido em Python, com auxilio da biblioteca PLY para a construção dos analisadores léxico e sintático.


## 2. Arquitetura do Sistema

O projeto seguiu uma arquitetura composta por várias etapas, sendo cada fase do processo de compilação tratada por um módulo específico. A implementação está dividida em: análise léxica, análise sintática, análise semântica e geração de código para a máquina virtual (VM).

A estrutura do projeto é constituida pelos seguintes módulos:
-   **<span>main.py</span>** - modulo responsável por coordenar todo o processo de compilação. Começa por ler o ficheiro de entrada, invoca cada uma das fases do compilador e escreve o resultado no ficheiro de saída.

-   **<span>lexer.py</span>** - responsável pela analise lexica. Faz uso do **``ply.lex``** para definir os tokens da linguagem Pascal. Quando é chamado na main é o responsável por converter o conteúdo do ficheiro de entrada numa sequencia de tokens necessaria para a fase seguinte.

-   **<span>parser.py</span>** - implementa a análise sintática com recurso a **``ply.yacc``**. Define a gramática da linguagem Pascal e construir a árvore sintática validando a estrutura do programa recebido.

-   **<span>semantic.py</span>** - responsável pela análise semântica. Verifica a coerência logica do código, como declarações de variáveis, compatibilidade de tipos e chamadas a funções ou procedimentos. Utiliza um dicionário interno para representar uma tabela de símbolos, guardando o tipo das variáveis e o seu valor.

-   **<span>code_generator.py</span>** - Gera código para uma máquina virtual (VM) baseada na árvore sintática validada. A tradução é dirigida pela sintaxe e segue um modelo de stack, onde as operações manipulam dados numa estrutura do tipo LIFO (*last in first out*).

## 3. Analisador Léxico
O analisador léxico tem como principal objetivo transformar o código-fonte num conjunto de tokens(palavras-chave, identificadores, operadores, etc.). Estes tokens servem como entrada para o analisador sintático. A implementação do analisador léxico foi realizada com auxilio da biblioteca PLY (ply.lex), que permite definir expressões regulares associadas a funções que identificam e classificam os padrões presentes no texto.

#### 3.1. Estrutura
**``lexer.py``** define uma classe **PascalLexer** que contém:
-   A lista de palavras-chave suportadas (ex: program, begin, end, var, if, then, writeln, readln, etc.);
-   A definição dos tokens, como ID, NUMBER, STRING, PLUS, MINUS, SEMI, COLON, etc.;
-   Regras com expressões regulares para reconhecer padrões associados a cada token, como:
    ```
    t_PLUS    = r'\+'
    t_MINUS   = r'-'
    t_ID      = r'[a-zA-Z_][a-zA-Z_0-9]*'
    t_NUMBER  = r'\d+'
    ```
-   Tratamento de comentários, espaços em branco, e linhas novas (para rastrear a linha dos erros);
-   Função de construção do lexer, que retorna o analisador pronto a ser usado pelo parser.

#### 3.2. Como funciona?
Quando o compilador é executado, o conteúdo do ficheiro de entrada é passado ao analisador léxico. Este percorre o texto e a cada correspondência com uma expressão regular, gera um token contendo um tipo, um valor e o número da linha onde foi encontrado.

## 4. Analisador sintático
O analisador sintático é responsável por validar a estrutura gramatical do programa. Recebe como entrada a sequência de tokens gerada pelo analisador léxico e verifica se estes seguem as regras da linguagem Pascal. Caso a sintaxe seja válida, é construída uma Árvore Sintática representando o conteúdo do programa. A implementação foi realizada com o auxilio à biblioteca PLY (ply.yacc), que permite definir as regras da gramática e associá-las a ações semânticas em Python.

#### 4.1. Estrutura
O ficheiro <span>parser.py</span> define:
-    O conjunto de regras de produção, que representam a gramática da linguagem Pascal;
-    Funções com nomes do tipo p_\<nome>, em que cada função define uma produção e constrói os nós da AST correspondentes;
-    A definição da precedência dos operadores, para evitar ambiguidades;
-    Tratamento de erros sintáticos, reportando mensagens com número de linha.

Exemplo de uma produção
```
def p_program(self, p):
        """program : PROGRAM ID SEMICOLON block DOT"""
        # p[2] = nome do programa
        p[0] = ('program', p[2], p[4])
```

Esta produção cria um nó da AST em forma de tuplo: ```('program', <nome>, <bloco>)```

Exemplo real:
```
program Ola;
begin
  writeln('Olá!');
end.
```

Gera o nó:
```
('program', 'Ola', <nó do bloco>)
```

#### 4.2. Gramática

A gramática implementada segue a estrutura do Pascal Standard:

**Estrutura Principal:**
```
program → PROGRAM ID SEMICOLON program_body DOT
program_body → function_declarations block | block
```

**Declarações de Funções:**
```
function_declarations → function_declarations function_declaration | function_declaration
function_declaration → FUNCTION ID LPAREN parameter_list RPAREN COLON type SEMICOLON block SEMICOLON
                     | FUNCTION ID LPAREN RPAREN COLON type SEMICOLON block SEMICOLON
parameter_list → parameter_list SEMICOLON parameter | parameter
parameter → id_list COLON type
```

**Blocos e Declarações:**
```
block → declarations compound_statement
declarations → VAR declaration_list | empty
declaration_list → declaration_list declaration | declaration
declaration → id_list COLON type SEMICOLON
id_list → id_list COMMA ID | ID
```

**Tipos:**
```
type → INTEGER_TYPE | REAL_TYPE | STRING_TYPE | BOOLEAN_TYPE | CHAR_TYPE | array_type
array_type → ARRAY LBRACKET INTEGER DOTDOT INTEGER RBRACKET OF type
```

**Comandos Compostos:**
```
compound_statement → BEGIN statement_list END
statement_list → statement_list SEMICOLON statement | statement
statement → assignment_statement | if_statement | while_statement | for_statement 
          | io_statement | compound_statement | empty
```

**Atribuições:**
```
assignment_statement → var ASSIGN expr_bool | ID ASSIGN expr_bool
```

**Estruturas de Controle:**
```
if_statement → IF expr_bool THEN statement | IF expr_bool THEN statement ELSE statement
while_statement → WHILE expr_bool DO statement
for_statement → FOR ID ASSIGN expr TO expr DO statement 
              | FOR ID ASSIGN expr DOWNTO expr DO statement
```

**Entrada e Saída:**
```
io_statement → WRITE LPAREN write_list RPAREN | WRITELN LPAREN write_list RPAREN
             | READ LPAREN read_list RPAREN | READLN LPAREN read_list RPAREN
             | WRITE | WRITELN | READ | READLN
write_list → write_list COMMA write_param | write_param
read_list → read_list COMMA var | var
write_param → expr_bool | STRING
```

**Expressões:**
```
expr_bool → expr_bool OR expr_and | expr_and
expr_and → expr_and AND expr_rel | expr_rel
expr_rel → expr op_rel expr | expr
op_rel → EQ | NEQ | LT | GT | LE | GE
expr → expr PLUS termo | expr MINUS termo | termo
termo → termo TIMES fator | termo DIVIDE fator | termo DIV fator | termo MOD fator | fator
fator → const | var | function_call | LPAREN expr_bool RPAREN | NOT fator | MINUS fator
```

**Chamadas de Função:**
```
function_call → LENGTH LPAREN expr_bool RPAREN | ID LPAREN argument_list RPAREN | ID LPAREN RPAREN
argument_list → argument_list COMMA expr_bool | expr_bool
```

**Constantes e Variáveis:**
```
const → INTEGER | REAL | STRING | CHAR_LITERAL | TRUE | FALSE
var → ID | ID LBRACKET expr RBRACKET
```

**Precedência de Operadores:**
```
precedence = (
    ('left', 'OR'),
    ('left', 'AND'),
    ('left', 'EQ', 'NEQ', 'LT', 'LE', 'GT', 'GE'),
    ('left', 'PLUS', 'MINUS'),
    ('left', 'TIMES', 'DIVIDE', 'DIV', 'MOD'),
    ('right', 'UMINUS', 'NOT'),
)
```

#### 4.3. Como funciona?
Durante a execução do compilador, o parser é invocado com a lista de tokens produzida pelo lexer. Se a sequência de tokens não corresponder a nenhuma regra da gramática, é emitido um erro sintático, com indicação da linha onde ocorreu a falha.

## 5. Analisador Semântico
O analisador semântico é responsável por verificar a coerência lógica do programa. Recebe como entrada a Árvore Sintática Abstrata (AST) gerada pelo parser e percorre-a para detetar erros semânticos.

#### 5.1 Estrutura
O ficheiro <span>semantic.py</span> define a classe SemanticAnalyzer, responsável por percorrer a AST e validar semanticamente cada nó.

Durante este percurso, são realizadas:
-    Verificações de tipo;
-    Validações de declarações de variáveis;
-    Atualizações e consultas ao dicionário de símbolos.

#### 5.2. Como funciona?
Ao encontrar uma instrução como por exemplo ```x := 5``` o compilador consulta o dicionário para verificar se x foi declarado previamente e qual o seu tipo.
Caso algum destes critérios não seja satisfeito, é gerado um erro semântico, e a compilação é interrompida.

**Exemplo de verificação semântica:**
```pascal
program teste;
var x: integer;
    y: real;
begin
  x := 5;        // OK: integer := integer
  y := x;        // OK: real := integer (conversão implícita)
  x := y;        // ERRO: integer := real (perda de precisão)
  writeln(z);    // ERRO: variável 'z' não declarada
end.
```

O analisador semântico detecta e reporta os erros:
- "Erro semântico: não é possível atribuir valor do tipo 'real' à variável 'x' do tipo 'integer'"
- "Erro semântico: variável 'z' não declarada"

## 6. Geração de Código para a Máquina Virtual
A fase final do compilador consiste na geração de código para uma máquina virtual (VM). Esta etapa é responsável por transformar a Árvore Sintática Abstrata (AST), previamente validada pelas fases anteriores, numa sequência de instruções simples e executáveis por uma VM com arquitetura baseada em pilha (stack-based).

#### 6.1. Input/Output 
As chamadas a readln e writeln são reconhecidas pelo analisador sintático como comandos especiais, pois, ao contrário das funções definidas pelo utilizador, não exigem declaração prévia. Estas funções fazem parte do conjunto de funcionalidades básicas da linguagem Pascal e são traduzidas diretamente para instruções específicas da máquina virtual.

**Exemplo:**
```pascal
program io_exemplo;
var x: integer;
begin
  readln(x);
  writeln('Valor lido: ', x);
end.
```

Gera o código VM:
```
READ
ATOI
STOREG 0
PUSHS "Valor lido: "
WRITES
PUSHG 0
WRITEI
WRITELN
```

#### 6.2. Condições
As instruções condicionais da linguagem Pascal, como ``if ... then`` ou ``if ... then ... else``, são transformadas pelo compilador em blocos de código que usam saltos e etiquetas (labels) na máquina virtual para controlar o fluxo de execução.

Durante a geração de código, a condição é avaliada primeiro e o seu resultado é colocado no topo da pilha. Em seguida, o compilador emite uma instrução JZ (jump if zero), que salta para o bloco else se o valor for 0.

**Exemplo:**
```pascal
if x > 5 then
  writeln('Grande')
else
  writeln('Pequeno');
```

Gera o código VM:
```
PUSHG 0    // x
PUSHI 5    // 5
SUP        // x > 5
JZ L1      // salta para else se falso
PUSHS "Grande"
WRITES
JUMP L2    // salta para fim
L1:
PUSHS "Pequeno"
WRITES
L2:
```

#### 6.3. Ciclos
Os ciclos while e for da linguagem Pascal são convertidos para instruções de máquina virtual que utilizam saltos e etiquetas para controlar a repetição.

No ciclo **``while``**, a condição é verificada no início de cada iteração. O compilador gera um label que marca o início do ciclo, seguido pelo código que avalia a condição, empilhando um booleano. Em caso de falso, a instrução JZ (jump if zero) faz saltar a execução para fora do ciclo.

**Exemplo de while:**
```pascal
while x > 0 do
begin
  writeln(x);
  x := x - 1;
end;
```

Gera o código VM:
```
L0:
PUSHG 0    // x
PUSHI 0    // 0
SUP        // x > 0
JZ L1      // sair se falso
PUSHG 0
WRITEI
PUSHG 0
PUSHI 1
SUB
STOREG 0   // x := x - 1
JUMP L0    // voltar ao início
L1:
```

O ciclo **``for``** segue uma lógica semelhante, mas com controle automático de uma variável de iteração.

**Exemplo de for:**
```pascal
for i := 1 to 5 do
  writeln(i);
```

Gera o código VM:
```
PUSHI 1
STOREG 0   // i := 1
L0:
PUSHG 0    // i
PUSHI 5    // 5
INFEQ      // i <= 5
JZ L1      // sair se falso
PUSHG 0
WRITEI
PUSHG 0
PUSHI 1
ADD
STOREG 0   // i := i + 1
JUMP L0
L1:
```

## 7. Fluxo de Execução
1.  Leitura do ficheiro com o código Pascal.  
    - o programa começa por receber como argumento um ficheiro contendo o código Pascal.
    - o conteúdo é lido e armazenado como uma string.

2.  Análise Léxica
    - a string é passada ao analisador léxico, que a converte numa sequência de tokens.

3.  Análise sintática
    - a sequência de tokens é analisada segundo as regras sintáticas da linguagem Pascal e é construida a árvore sintática, que representa a lógica do programa.

4.  Análise semântica
    - a árvore sintática é percorrida detetando erros como variáveis não declaradas, incompatibilidades de tipo ou uso incorreto de funções.

5.  Geração de código
    - com base na árvore sintática já validada é gerado código para ser utilizado na VM.

## 8. Exemplos

### Exemplo 1: Array e Ciclo For

**Código Pascal:**
```pascal
program arrays;
var nums: array[1..3] of integer;
var i: integer;
begin
  for i := 1 to 3 do
    nums[i] := i * 2;
  
  for i := 1 to 3 do
    writeln(nums[i]);
end.
```

**Código VM Gerado:**
```
// Programa: arrays

// Declaração do array nums
PUSHI 3
ALLOCN
STOREG 0

// Declaração da variável i
PUSHI 0
STOREG 1
START

// for i := 1 to 3 do nums[i] := i * 2
PUSHI 1
STOREG 1
L0:
PUSHG 1
PUSHI 3
INFEQ
JZ L1
PUSHG 0
PUSHG 1
PUSHI 1
SUB
PUSHG 1
PUSHI 2
MUL
STOREN
PUSHG 1
PUSHI 1
ADD
STOREG 1
JUMP L0
L1:

// for i := 1 to 3 do writeln(nums[i])
PUSHI 1
STOREG 1
L2:
PUSHG 1
PUSHI 3
INFEQ
JZ L3
PUSHG 0
PUSHG 1
PUSHI 1
SUB
LOADN
WRITEI
WRITELN
PUSHG 1
PUSHI 1
ADD
STOREG 1
JUMP L2
L3:

STOP
```

## 9. Como Executar
    python3 main.py <pascal_file>

Flags opcionais:
-   ``-o <output_file>``: define o ficheiro de saída.
-   ``-d``: ativa o modo de *debug*, onde são imprimidos tokens, árvore sintática e o código gerado.

## 9. Conclusão

O desenvolvimento deste compilador para a linguagem Pascal Standard proporcionou uma compreensão aprofundada do processo de tradução de linguagens de alto nível para instruções de baixo nível executáveis. Através da implementação sequencial das fases de análise léxica, sintática e semântica, bem como da geração de código, foi possível aplicar conceitos fundamentais de compiladores.

Cada etapa revelou desafios específicos: desde a definição rigorosa dos padrões lexicais e sintáticos, passando pela construção de uma árvore sintática abstrata e pela validação semântica, até à tradução final para código de máquina virtual em stack.

Este projeto serviu como uma base sólida para possíveis extensões futuras, como otimizações de código e suporte a mais funcionalidades da linguagem Pascal.