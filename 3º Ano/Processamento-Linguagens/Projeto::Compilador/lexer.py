import re
import ply.lex as lex

class PascalLexer:
    """
    Classe para o analisador lexico de Pascal usando a biblioteca PLY
    """

    # Lista de tokens que o nosso lexer vai reconhecer
    tokens = [
        # Identificadores e literais
        'ID',  # Identificadores (nomes de variáveis, funções, etc.)
        'INTEGER',  # Números inteiros
        'REAL',  # Números de ponto flutuante
        'STRING',  # Strings (textos entre aspas simples)

        # Operadores
        'PLUS',  # Operador de adição (+)
        'MINUS',  # Operador de subtração (-)
        'TIMES',  # Operador de multiplicação (*)
        'DIVIDE',  # Operador de divisão (/)
        'ASSIGN',  # Operador de atribuição (:=)

        # Operadores de comparação
        'EQ',  # Igual (=)
        'NEQ',  # Diferente (<>)
        'LT',  # Menor que (<)
        'GT',  # Maior que (>)
        'LE',  # Menor ou igual (<=)
        'GE',  # Maior ou igual (>=)

        # Delimitadores
        'LPAREN',  # Parêntese esquerdo (()
        'RPAREN',  # Parêntese direito ())
        'LBRACKET',  # Colchete esquerdo ([)
        'RBRACKET',  # Colchete direito (])
        'COMMA',  # Vírgula (,)
        'SEMICOLON',  # Ponto e vírgula (;)
        'COLON',  # Dois pontos (:)
        'DOT',  # Ponto (.)
        'DOTDOT',  # Intervalo (..)

        'PROGRAM', 'VAR', 'BEGIN', 'END', 'IF', 'THEN', 'ELSE', 'WHILE', 'DO',
        'FOR', 'TO', 'DOWNTO', 'INTEGER_TYPE', 'REAL_TYPE', 'STRING_TYPE', 'BOOLEAN_TYPE',
        'CHAR_TYPE',  # Tipo char
        'ARRAY', 'OF', 'WRITE', 'WRITELN', 'READ', 'READLN', 'OR', 'AND', 'NOT',
        'DIV', 'MOD', 'TRUE', 'FALSE', 'FUNCTION', 'PROCEDURE',
        'LENGTH',  # Função built-in LENGTH
        'CHAR_LITERAL',  # Literais de caractere como 'A'
    ]

    t_ignore = ' \t'  # Mantém original, mas quebras de linha tratadas separadamente

    # Definir regras para tokens simples com expressões regulares

    # Operadores
    t_PLUS = r'\+'  # O caractere +
    t_MINUS = r'-'  # O caractere -
    t_TIMES = r'\*'  # O caractere *
    t_DIVIDE = r'/'  # O caractere /
    t_ASSIGN = r':='  # A sequência :=

    # Operadores de comparação
    t_EQ = r'='  # O caractere =
    t_NEQ = r'<>'  # A sequência <>
    t_LT = r'<'  # O caractere <
    t_GT = r'>'  # O caractere >
    t_LE = r'<='  # A sequência <=
    t_GE = r'>='  # A sequência >=

    # Delimitadores
    t_LPAREN = r'\('  # O caractere (
    t_RPAREN = r'\)'  # O caractere )
    t_LBRACKET = r'\['  # O caractere [
    t_RBRACKET = r'\]'  # O caractere ]
    t_COMMA = r','  # O caractere ,
    t_SEMICOLON = r';'  # O caractere ;
    t_COLON = r':'  # O caractere :
    t_DOT = r'\.'  # O caractere .
    t_DOTDOT = r'\.\.'  # A sequência ..

    def t_newline(self, t):
        r'\n+'  # Um ou mais caracteres de nova linha
        t.lexer.lineno += len(t.value)  # Atualiza o contador de linhas
        #  Não retorna token, quebras de linha são ignoradas


    def t_LENGTH(self, t):
        r'length'
        t.value = t.value.lower()
        return t

    def t_PROGRAM(self, t):
        r'program'
        t.value = t.value.lower()
        return t

    def t_PROCEDURE(self, t):
        r'procedure'
        t.value = t.value.lower()
        return t

    def t_FUNCTION(self, t):
        r'function'
        t.value = t.value.lower()
        return t

    def t_VAR(self, t):
        r'var'
        t.value = t.value.lower()
        return t

    def t_BEGIN(self, t):
        r'begin'
        t.value = t.value.lower()
        return t

    def t_END(self, t):
        r'end'
        t.value = t.value.lower()
        return t

    def t_IF(self, t):
        r'if'
        t.value = t.value.lower()
        return t

    def t_THEN(self, t):
        r'then'
        t.value = t.value.lower()
        return t

    def t_ELSE(self, t):
        r'else'
        t.value = t.value.lower()
        return t

    def t_WHILE(self, t):
        r'while'
        t.value = t.value.lower()
        return t

    def t_FOR(self, t):
        r'for'
        t.value = t.value.lower()
        return t

    def t_DOWNTO(self, t):
        r'downto'
        t.value = t.value.lower()
        return t

    def t_TO(self, t):
        r'to'
        t.value = t.value.lower()
        return t

    def t_DO(self, t):
        r'do'
        t.value = t.value.lower()
        return t

    # Tipos de dados - ordem específica para evitar conflitos
    def t_INTEGER_TYPE(self, t):
        r'integer'
        t.value = t.value.lower()
        return t

    def t_BOOLEAN_TYPE(self, t):
        r'boolean'
        t.value = t.value.lower()
        return t

    def t_CHAR_TYPE(self, t):
        r'char'
        t.value = t.value.lower()
        return t

    def t_STRING_TYPE(self, t):
        r'string'
        t.value = t.value.lower()
        return t

    def t_REAL_TYPE(self, t):
        r'real'
        t.value = t.value.lower()
        return t

    def t_ARRAY(self, t):
        r'array'
        t.value = t.value.lower()
        return t

    def t_OF(self, t):
        r'of'
        t.value = t.value.lower()
        return t

    # IO operations - ordem específica
    def t_WRITELN(self, t):
        r'writeln'
        t.value = t.value.lower()
        return t

    def t_WRITE(self, t):
        r'write'
        t.value = t.value.lower()
        return t

    def t_READLN(self, t):
        r'readln'
        t.value = t.value.lower()
        return t

    def t_READ(self, t):
        r'read'
        t.value = t.value.lower()
        return t

    # Operadores lógicos
    def t_OR(self, t):
        r'or'
        t.value = t.value.lower()
        return t

    def t_AND(self, t):
        r'and'
        t.value = t.value.lower()
        return t

    def t_NOT(self, t):
        r'not'
        t.value = t.value.lower()
        return t

    # Operadores aritméticos especiais
    def t_DIV(self, t):
        r'div'
        t.value = t.value.lower()
        return t

    def t_MOD(self, t):
        r'mod'
        t.value = t.value.lower()
        return t

    # Valores booleanos
    def t_TRUE(self, t):
        r'true'
        t.value = t.value.lower()
        return t

    def t_FALSE(self, t):
        r'false'
        t.value = t.value.lower()
        return t

    # ID deve vir DEPOIS de todas as palavras-chave
    def t_ID(self, t):
        r'[a-zA-Z][a-zA-Z0-9_]*'  # Começa com letra, seguida de letras, números ou _
        return t

    # Regra para números reais, deve vir antes da regra para inteiros
    def t_REAL(self, t):
        r'\d+\.\d+'  # Um ou mais números, seguido de ponto e mais números
        t.value = float(t.value)
        return t

    # Regra para números inteiros
    def t_INTEGER(self, t):
        r'-?\d+'  # Um ou mais dígitos
        t.value = int(t.value)
        return t

    # Regra para caracteres únicos
    def t_CHAR_LITERAL(self, t):
        r"'[^']'"  # Um único caractere entre aspas simples
        t.value = t.value[1:-1]  # Remove as aspas
        return t

    # Regra para strings (deve vir depois de CHAR_LITERAL)
    def t_STRING(self, t):
        r"'[^']*'"  # Qualquer sequência entre aspas simples (incluindo vazio)
        # Se for um único caractere, é um CHAR_LITERAL, não STRING
        if len(t.value) == 3:  # 'X' = 3 caracteres
            t.type = 'CHAR_LITERAL'
            t.value = t.value[1:-1]
        else:
            t.value = t.value[1:-1]  # Remove as aspas para strings
        return t

    # Regra para comentários entre { ... }
    def t_COMMENT_BRACES(self, t):
        r'\{[^}]*\}'  # Qualquer coisa entre { e }
        t.lexer.lineno += t.value.count('\n')  # Atualiza contador de linhas
        # Comentários são ignorados, não retornam

    # Regra para comentários entre (* ... *)
    def t_COMMENT_PARENTHESES(self, t):
        r'\(\*[\s\S]*?\*\)'  # Qualquer coisa entre (* e *)
        t.lexer.lineno += t.value.count('\n')  # Atualiza contador de linhas
        # Comentários são ignorados, não retornam

    # Regra para tratar erros
    def t_error(self, t):
        print(f"Erro léxico: caractere ilegal '{t.value[0]}' na linha {t.lexer.lineno}")
        t.lexer.skip(1)  # Salta o caractere e continua

    # Método para construir o lexer
    def build(self, **kwargs):
        """
        Constrói o analisador léxico
        :return: o objeto lexer construído
        """
        if 'reflags' not in kwargs:
            kwargs['reflags'] = re.IGNORECASE
        self.lexer = lex.lex(module=self, **kwargs)
        return self.lexer

    # Método para tokenizar uma string de entrada
    def tokenize(self, data):
        """
        Tokeniza uma string de entrada e retorna a lista de tokens

        :param data (str): o código pascal a ser tokenizado
        :return: list: lista de tokens encontrados
        """
        # Reinicia o lexer e define entrada
        self.lexer.input(data)

        # Recebe todos os tokens
        tokens = []
        while True:
            token = self.lexer.token()
            if not token:
                break
            tokens.append(token)

        return tokens