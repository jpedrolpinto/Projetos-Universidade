import ply.yacc as yacc
from lexer import PascalLexer

tokens = PascalLexer.tokens

class PascalParser:
    """
    Classe para o analisador sintático de Pascal usando a biblioteca PLY.
    """

    def __init__(self):
        """
        Inicializa o parser.
        """
        # Obtém os tokens do lexer
        self.lexer = PascalLexer()
        self.tokens = self.lexer.tokens

        # Inicializa o parser
        self.parser = None

    def build(self, **kwargs):
        """
        Constrói o parser.

        Returns:
            O objeto parser construído.
        """
        if 'debug' not in kwargs:
            kwargs['debug'] = False
        if 'start' not in kwargs:
            kwargs['start'] = 'program'

        # CRÍTICO: Força regeneração do parser
        if 'write_tables' not in kwargs:
            kwargs['write_tables'] = True

        self.parser = yacc.yacc(module=self, **kwargs)
        return self.parser

    def p_program(self, p):
        """program : PROGRAM ID SEMICOLON program_body DOT"""
        # p[2] = nome do programa, p[4] = corpo do programa
        if len(p[4]) == 1:
            # Apenas bloco principal
            p[0] = ('program', p[2], p[4][0])
        else:
            # Funções + bloco principal
            p[0] = ('program', p[2], p[4][0], p[4][1])

    def p_program_body(self, p):
        """program_body : function_declarations block
                        | block"""
        if len(p) == 3:
            # Com funções
            p[0] = [p[1], p[2]]
        else:
            # Apenas bloco
            p[0] = [p[1]]

    def p_function_declarations(self, p):
        """function_declarations : function_declarations function_declaration
                                 | function_declaration"""
        if len(p) > 2:
            p[1].append(p[2])
            p[0] = p[1]
        else:
            p[0] = [p[1]]

    def p_function_declaration(self, p):
        """function_declaration : FUNCTION ID LPAREN parameter_list RPAREN COLON type SEMICOLON block SEMICOLON
                                | FUNCTION ID LPAREN RPAREN COLON type SEMICOLON block SEMICOLON"""
        if len(p) == 11:
            # Função com parâmetros
            p[0] = ('function_declaration', p[2], p[4], p[7], p[9])
        else:
            # Função sem parâmetros
            p[0] = ('function_declaration', p[2], [], p[6], p[8])

    def p_parameter_list(self, p):
        """parameter_list : parameter_list SEMICOLON parameter
                          | parameter"""
        if len(p) > 2:
            p[1].append(p[3])
            p[0] = p[1]
        else:
            p[0] = [p[1]]

    def p_parameter(self, p):
        """parameter : id_list COLON type"""
        p[0] = ('parameter', p[1], p[3])

    def p_block(self, p):
        """block : declarations compound_statement"""
        p[0] = ('block', p[1], p[2])

    def p_declarations(self, p):
        """declarations : VAR declaration_list
                        | empty"""
        if len(p) > 2:  # Se houver declarações
            p[0] = ('declarations', p[2])
        else:
            p[0] = ('declarations', [])

    def p_declaration_list(self, p):
        """declaration_list : declaration_list declaration
                            | declaration"""
        if len(p) > 2:  # Se for uma lista de declarações
            p[1].append(p[2])
            p[0] = p[1]
        else:
            p[0] = [p[1]]

    def p_declaration(self, p):
        """declaration : id_list COLON type SEMICOLON"""
        p[0] = ('declaration', p[1], p[3])

    def p_id_list(self, p):
        """id_list : id_list COMMA ID
                   | ID"""
        if len(p) > 2:  # Se for uma lista de IDs
            p[1].append(p[3])
            p[0] = p[1]
        else:
            p[0] = [p[1]]

    def p_type(self, p):
        """type : INTEGER_TYPE
                | REAL_TYPE
                | STRING_TYPE
                | BOOLEAN_TYPE
                | CHAR_TYPE
                | array_type"""
        if p.slice[1].type in ['INTEGER_TYPE', 'REAL_TYPE', 'STRING_TYPE', 'BOOLEAN_TYPE', 'CHAR_TYPE']:
            p[0] = p[1].lower()
        else:
            p[0] = p[1]

    def p_array_type(self, p):
        """array_type : ARRAY LBRACKET INTEGER DOTDOT INTEGER RBRACKET OF type"""
        p[0] = ('array_type', (p[3], p[5]), p[8])

    def p_compound_statement(self, p):
        """compound_statement : BEGIN statement_list END"""
        p[0] = ('compound', p[2])

    def p_statement_list(self, p):
        """statement_list : statement_list SEMICOLON statement
                          | statement"""
        if len(p) > 2:  # Se for uma lista de instruções
            if p[3] is not None:  # Só adiciona se não for None
                p[1].append(p[3])
            p[0] = p[1]
        else:
            p[0] = [p[1]] if p[1] is not None else []

    def p_empty(self, p):
        """empty :"""
        p[0] = None

    def p_statement(self, p):
        """statement : assignment_statement
                     | if_statement
                     | while_statement
                     | for_statement
                     | io_statement
                     | compound_statement
                     | empty"""
        p[0] = p[1]

    def p_assignment_statement(self, p):
        """assignment_statement : var ASSIGN expr_bool
                                | ID ASSIGN expr_bool"""
        if isinstance(p[1], str):
            # Atribuição simples: ID := expr
            p[0] = ('assign', p[1], p[3])
        else:
            # Atribuição a array: array[index] := expr
            p[0] = ('assign', p[1], p[3])

    def p_if_statement(self, p):
        """if_statement : IF expr_bool THEN statement
                        | IF expr_bool THEN statement ELSE statement"""
        if len(p) > 5:  # Se tiver cláusula else
            p[0] = ('if', p[2], p[4], p[6])
        else:
            p[0] = ('if', p[2], p[4], None)

    def p_while_statement(self, p):
        """while_statement : WHILE expr_bool DO statement"""
        p[0] = ('while', p[2], p[4])

    def p_for_statement(self, p):
        """for_statement : FOR ID ASSIGN expr TO expr DO statement
                         | FOR ID ASSIGN expr DOWNTO expr DO statement"""
        if p[5] == 'to':
            p[0] = ('for_to', p[2], p[4], p[6], p[8])
        else:  # downto
            p[0] = ('for_downto', p[2], p[4], p[6], p[8])

    def p_io_statement(self, p):
        """io_statement : WRITE LPAREN write_list RPAREN
                        | WRITELN LPAREN write_list RPAREN
                        | READ LPAREN read_list RPAREN
                        | READLN LPAREN read_list RPAREN
                        | WRITE
                        | WRITELN
                        | READ
                        | READLN"""
        if len(p) > 2:  # Se tiver parâmetros
            p[0] = (p[1], p[3])
        else:
            p[0] = (p[1], [])

    def p_write_list(self, p):
        """write_list : write_list COMMA write_param
                      | write_param"""
        if len(p) > 2:  # Se for uma lista de parâmetros
            p[1].append(p[3])
            p[0] = p[1]
        else:
            p[0] = [p[1]]

    def p_read_list(self, p):
        """read_list : read_list COMMA var
                     | var"""
        if len(p) > 2:  # Se for uma lista de variáveis
            p[1].append(p[3])
            p[0] = p[1]
        else:
            p[0] = [p[1]]

    def p_write_param(self, p):
        """write_param : expr_bool
                       | STRING"""
        if p.slice[1].type == 'STRING':
            p[0] = ('STRING_LITERAL', p[1])
        else:
            p[0] = p[1]

    def p_expr_bool(self, p):
        """expr_bool : expr_bool OR expr_and
                     | expr_and"""
        if len(p) == 2:
            p[0] = p[1]
        else:
            p[0] = ('or', p[1], p[3])

    def p_expr_and(self, p):
        """expr_and : expr_and AND expr_rel
                    | expr_rel"""
        if len(p) == 2:
            p[0] = p[1]
        else:
            p[0] = ('and', p[1], p[3])

    def p_expr_rel(self, p):
        """expr_rel : expr op_rel expr
                    | expr"""
        if len(p) == 2:
            p[0] = p[1]
        else:
            p[0] = (p[2], p[1], p[3])

    def p_op_rel(self, p):
        """op_rel : EQ
                  | NEQ
                  | LT
                  | GT
                  | LE
                  | GE"""
        p[0] = p[1]

    def p_expr(self, p):
        """expr : expr PLUS termo
                | expr MINUS termo
                | termo"""
        if len(p) == 2:
            p[0] = p[1]
        else:
            p[0] = (p[2], p[1], p[3])

    def p_termo(self, p):
        """termo : termo TIMES fator
                 | termo DIVIDE fator
                 | termo DIV fator
                 | termo MOD fator
                 | fator"""
        if len(p) == 2:
            p[0] = p[1]
        else:
            p[0] = (p[2], p[1], p[3])

    def p_fator(self, p):
        """fator : const
                 | var
                 | function_call
                 | LPAREN expr_bool RPAREN
                 | NOT fator
                 | MINUS fator %prec UMINUS"""
        if len(p) == 2:
            p[0] = p[1]
        elif len(p) == 3:  # NOT fator ou MINUS fator
            if p[1] == 'not':
                p[0] = ('not', p[2])
            else:  # MINUS unário
                p[0] = ('-', p[2])
        else:  # parênteses
            p[0] = p[2]

    def p_function_call(self, p):
        """function_call : LENGTH LPAREN expr_bool RPAREN
                         | ID LPAREN argument_list RPAREN
                         | ID LPAREN RPAREN"""
        if p[1] == 'length':
            p[0] = ('function_call', 'length', [p[3]])
        elif len(p) == 5:
            # Chamada de função com argumentos
            p[0] = ('function_call', p[1], p[3])
        else:
            # Chamada de função sem argumentos
            p[0] = ('function_call', p[1], [])

    def p_argument_list(self, p):
        """argument_list : argument_list COMMA expr_bool
                         | expr_bool"""
        if len(p) > 2:
            p[1].append(p[3])
            p[0] = p[1]
        else:
            p[0] = [p[1]]

    def p_const(self, p):
        """const : INTEGER
                 | REAL
                 | STRING
                 | CHAR_LITERAL
                 | TRUE
                 | FALSE"""
        if p.slice[1].type == 'TRUE':
            p[0] = 'true'
        elif p.slice[1].type == 'FALSE':
            p[0] = 'false'
        elif p.slice[1].type == 'CHAR_LITERAL':
            p[0] = ('char_literal', p[1])
        else:
            p[0] = p[1]

    def p_var(self, p):
        """var : ID
               | ID LBRACKET expr RBRACKET"""
        if len(p) == 2:
            p[0] = p[1]
        else:  # acesso a array ou string
            p[0] = ('array_access', p[1], p[3])

    # PRECEDÊNCIA
    precedence = (
        ('left', 'OR'),
        ('left', 'AND'),
        ('left', 'EQ', 'NEQ', 'LT', 'LE', 'GT', 'GE'),
        ('left', 'PLUS', 'MINUS'),
        ('left', 'TIMES', 'DIVIDE', 'DIV', 'MOD'),
        ('right', 'UMINUS', 'NOT'),
    )

    def p_error(self, p):
        if p:
            print(f"Erro de sintaxe: token inesperado '{p.value}' (tipo: {p.type}) na linha {p.lineno}")
            print(f"  Posição: {p.lexpos}")
        else:
            print("Erro de sintaxe: fim de arquivo inesperado")

    def parse(self, data):
        """
        Analisa um programa Pascal.

        Args:
            data (str): Código fonte do programa.

        Returns:
            A árvore sintática do programa ou None em caso de erro.
        """
        # Constrói o lexer se ainda não foi construído
        if not hasattr(self, 'lexer_instance'):
            self.lexer_instance = self.lexer.build()

        # Reset do lexer antes de cada parse
        self.lexer_instance.lineno = 1

        # Analisa o programa
        try:
            result = self.parser.parse(data, lexer=self.lexer_instance, debug=False)
            return result
        except Exception as e:
            print(f"Erro durante o parsing: {e}")
            import traceback
            traceback.print_exc()
            return None


# Função de parse para uso no main.py
def parse(data, lexer):
    parser = PascalParser()
    parser.build()
    return parser.parse(data)