# -*- coding: utf-8 -*-
"""
Analisador Semântico para a linguagem Pascal Standard.
"""


class SemanticError(Exception):
    """Exceção para erros semânticos."""
    pass


class SemanticAnalyzer:
    """
    Classe para o analisador semântico de Pascal.
    """

    def __init__(self):
        """
        Inicializa o analisador semântico.
        """
        # Lista de escopos (cada escopo é um dicionário)
        # O primeiro escopo é sempre o escopo global
        self.scopes = [{}]
        self.errors = []
        self.current_function = None  # Para rastrear a função atual

    def add_symbol(self, name, symbol_type, data_type=None, params=None):
        """
        Adiciona um símbolo ao escopo atual.

        Args:
            name: Nome do símbolo
            symbol_type: Tipo do símbolo ('var', 'function', 'program', 'parameter')
            data_type: Tipo de dados (ex: 'integer', 'real', etc.)
            params: Parâmetros da função (se aplicável)
        """
        symbol = {
            'name': name,
            'kind': symbol_type,
            'type': data_type,
            'params': params
        }
        self.scopes[-1][name] = symbol

    def lookup_symbol(self, name, current_scope_only=False):
        """
        Procura um símbolo nos escopos.

        Args:
            name: Nome do símbolo
            current_scope_only: Se True, procura apenas no scope atual

        Returns:
            dict: Símbolo encontrado ou None
        """
        if current_scope_only:
            return self.scopes[-1].get(name)

        # Procura do escopo mais interno para o mais externo
        for scope in reversed(self.scopes):
            if name in scope:
                return scope[name]
        return None

    def enter_scope(self):
        """Entra em um novo escopo."""
        self.scopes.append({})

    def exit_scope(self):
        """Sai do escopo atual."""
        if len(self.scopes) > 1:
            self.scopes.pop()

    def _get_type_string(self, type_info):
        """
        Extrai uma string de tipo de uma informação de tipo que pode ser string ou tupla.

        Args:
            type_info: Informação de tipo (str ou tuple)

        Returns:
            str: String do tipo
        """
        if isinstance(type_info, str):
            return type_info
        elif isinstance(type_info, tuple):
            if type_info[0] == 'array_type':
                return type_info[2]  # Tipo base do array
            else:
                return str(type_info)
        else:
            return str(type_info)

    def analyze(self, ast):
        """
        Analisa a árvore sintática e verifica a validade semântica.

        Args:
            ast: A árvore sintática abstrata (AST) do programa.

        Returns:
            bool: True se a análise foi bem-sucedida, False caso contrário.
        """
        try:
            self._analyze_node(ast)

            # Filtrar erros relacionados a strings literais
            filtered_errors = []
            for error in self.errors:
                if not (error.startswith("Erro semântico: variável '") and
                        (error.endswith("' não declarada") and
                         ("'" in error or '"' in error))):
                    filtered_errors.append(error)

            self.errors = filtered_errors
            return len(self.errors) == 0
        except SemanticError as e:
            self.errors.append(str(e))
            return False

    def _analyze_node(self, node):
        """
        Analisa um nó da árvore sintática.

        Args:
            node: Um nó da árvore sintática.

        Raises:
            SemanticError: Se um erro semântico for encontrado.
        """
        if not node:
            return

        # Se o nó for uma tupla, verifica o tipo de nó e chama a função apropriada
        if isinstance(node, tuple):
            node_type = node[0]

            # Diferentes tipos de nós da AST
            if node_type == 'program':
                self._analyze_program(node)
            elif node_type == 'function_declaration':
                self._analyze_function_declaration(node)
            elif node_type == 'block':
                self._analyze_block(node)
            elif node_type == 'declarations':
                self._analyze_declarations(node)
            elif node_type == 'declaration':
                self._analyze_declaration(node)
            elif node_type == 'compound':
                self._analyze_compound_statement(node)
            elif node_type == 'assign':
                self._analyze_assignment(node)
            elif node_type == 'function_return':
                self._analyze_function_return(node)
            elif node_type == 'if':
                self._analyze_if_statement(node)
            elif node_type == 'while':
                self._analyze_while_statement(node)
            elif node_type == 'for_to' or node_type == 'for_downto':
                self._analyze_for_statement(node)
            elif node_type in ('writeln', 'write', 'read', 'readln'):
                self._analyze_io_statement(node)
            elif node_type == 'array_access':
                self._analyze_array_access(node)
            elif node_type == 'function_call':
                self._analyze_function_call(node)
            elif node_type == 'char_literal':
                # Literais de caractere são válidos
                pass
            # Operações aritméticas, relacionais e lógicas
            elif node_type in ('+', '-', '*', '/', 'div', 'mod', '=', '<>', '<', '>', '<=', '>=', 'and', 'or', 'not'):
                self._analyze_operation(node)
            else:
                # Para outros tipos de nós, analisa recursivamente os filhos
                for child in node[1:]:
                    self._analyze_node(child)

    def _analyze_program(self, node):
        """
        Analisa o nó 'program'.

        Args:
            node: Nó 'program' da AST.
        """
        # Formato: ('program', program_name, block) ou ('program', program_name, functions, block)
        program_name = node[1]

        if len(node) == 3:
            # Programa sem funções
            block = node[2]
            functions = []
        else:
            # Programa com funções
            functions = node[2]
            block = node[3]

        # Adiciona o nome do programa ao escopo global
        self.add_symbol(program_name, 'program')

        # Analisa as funções primeiro
        for function in functions:
            self._analyze_node(function)

        # Analisa o bloco do programa
        self._analyze_node(block)

    def _analyze_function_declaration(self, node):
        """
        Analisa declaração de função.
        """
        # Formato: ('function_declaration', name, parameters, return_type, block)
        func_name = node[1]
        parameters = node[2]
        return_type = node[3]
        block = node[4]

        # Registra a função no escopo global
        self.add_symbol(func_name, 'function', return_type, parameters)

        # Cria novo escopo para a função
        self.enter_scope()
        self.current_function = func_name

        # Adiciona parâmetros ao escopo da função
        for param in parameters:
            param_names = param[1]  # lista de nomes
            param_type = param[2]  # tipo
            for param_name in param_names:
                self.add_symbol(param_name, 'parameter', param_type)

        # Analisa o bloco da função
        self._analyze_node(block)

        # Sai do escopo da função
        self.exit_scope()
        self.current_function = None

    def _analyze_function_return(self, node):
        """
        Analisa atribuição de valor de retorno.
        """
        # Formato: ('function_return', function_name, expression)
        func_name = node[1]
        expression = node[2]

        if self.current_function != func_name:
            error_msg = f"Erro semântico: atribuição de retorno '{func_name}' fora da função correspondente"
            self.errors.append(error_msg)

        # Analisa a expressão
        self._analyze_expression(expression)

    def _analyze_function_call(self, node):
        """
        Analisa chamada de função.
        """
        # Formato: ('function_call', function_name, arguments)
        func_name = node[1]
        arguments = node[2]

        if func_name == 'length':
            # Função built-in
            if len(arguments) != 1:
                error_msg = "Erro semântico: função length() espera exatamente 1 argumento"
                self.errors.append(error_msg)
            else:
                arg_type = self._analyze_expression(arguments[0])
                if arg_type and arg_type not in ('string', 'char'):
                    error_msg = f"Erro semântico: função length() espera string ou char, mas recebeu '{arg_type}'"
                    self.errors.append(error_msg)
        else:
            # Função definida pelo usuário
            func_symbol = self.lookup_symbol(func_name)
            if not func_symbol:
                error_msg = f"Erro semântico: função '{func_name}' não declarada"
                self.errors.append(error_msg)
            elif func_symbol['kind'] != 'function':
                error_msg = f"Erro semântico: '{func_name}' não é uma função"
                self.errors.append(error_msg)
            else:
                # Verifica argumentos
                expected_params = len(func_symbol['params']) if func_symbol['params'] else 0
                actual_args = len(arguments)
                if expected_params != actual_args:
                    error_msg = f"Erro semântico: função '{func_name}' espera {expected_params} argumentos, mas recebeu {actual_args}"
                    self.errors.append(error_msg)

                # Analisa cada argumento
                for arg in arguments:
                    self._analyze_expression(arg)

    def _analyze_block(self, node):
        """
        Analisa o nó 'block'.

        Args:
            node: Nó 'block' da AST.
        """
        # Formato: ('block', declarations, compound_statement)
        declarations = node[1]
        compound_statement = node[2]

        # Analisa as declarações
        self._analyze_node(declarations)

        # Analisa o bloco de instruções
        self._analyze_node(compound_statement)

    def _analyze_declarations(self, node):
        """
        Analisa o nó 'declarations'.

        Args:
            node: Nó 'declarations' da AST.
        """
        # Formato: ('declarations', [declaration1, declaration2, ...])
        declarations = node[1]

        # Se declarations é uma lista, analisa cada declaração
        if isinstance(declarations, list):
            for declaration in declarations:
                self._analyze_node(declaration)

    def _analyze_declaration(self, node):
        """
        Analisa o nó 'declaration'.

        Args:
            node: Nó 'declaration' da AST.
        """
        # Formato: ('declaration', [id1, id2, ...], type)
        var_names = node[1]
        var_type = node[2]

        # Adiciona cada variável ao escopo atual
        for var_name in var_names:
            if self.lookup_symbol(var_name, current_scope_only=True):
                error_msg = f"Erro semântico: variável '{var_name}' já declarada neste escopo"
                self.errors.append(error_msg)
            else:
                self.add_symbol(var_name, 'var', var_type)

    def _analyze_compound_statement(self, node):
        """
        Analisa o nó 'compound'.

        Args:
            node: Nó 'compound' da AST.
        """
        # Formato: ('compound', [statement1, statement2, ...])
        statements = node[1]

        # Analisa cada instrução
        for statement in statements:
            self._analyze_node(statement)

    def _analyze_assignment(self, node):
        """
        Analisa o nó 'assign'.

        Args:
            node: Nó 'assign' da AST.
        """
        # Formato: ('assign', variable_name_or_array_access, expression)
        var_or_array = node[1]
        expression = node[2]

        # Verifica se é atribuição a array ou variável simples
        if isinstance(var_or_array, tuple) and var_or_array[0] == 'array_access':
            # É uma atribuição a array: array[index] := expr
            self._analyze_array_access(var_or_array)

            # Analisa a expressão do valor a ser atribuído
            expr_type = self._analyze_expression(expression)

        else:
            # É uma atribuição simples: var := expr
            var_name = var_or_array

            # Verifica se a variável existe
            var_symbol = self.lookup_symbol(var_name)
            if not var_symbol:
                error_msg = f"Erro semântico: variável '{var_name}' não declarada"
                self.errors.append(error_msg)
                return

            # Analisa a expressão
            expr_type = self._analyze_expression(expression)

            # Obtém o tipo da variável (pode ser string ou tupla para arrays)
            var_type_info = var_symbol['type']
            var_type_str = self._get_type_string(var_type_info)

            # Verifica compatibilidade de tipos
            if expr_type and var_type_str != expr_type:
                if not self._is_compatible_type(var_type_str, expr_type):
                    error_msg = f"Erro semântico: não é possível atribuir valor do tipo '{expr_type}' à variável '{var_name}' do tipo '{var_type_str}'"
                    self.errors.append(error_msg)

    def _analyze_array_access(self, node):
        """
        Analisa acesso a array ou string.

        Args:
            node: Nó 'array_access' da AST.
        """
        # Formato: ('array_access', name, index_expr)
        name = node[1]
        index_expr = node[2]

        # Verifica se a variável existe
        symbol = self.lookup_symbol(name)
        if not symbol:
            error_msg = f"Erro semântico: variável '{name}' não declarada"
            self.errors.append(error_msg)
            return

        # Verifica se é um array ou string
        if isinstance(symbol['type'], tuple) and symbol['type'][0] == 'array_type':
            # É um array
            pass
        elif symbol['type'] == 'string':
            # É uma string (acesso a caractere)
            pass
        else:
            error_msg = f"Erro semântico: '{name}' não é um array nem uma string"
            self.errors.append(error_msg)
            return

        # Analisa a expressão do índice
        index_type = self._analyze_expression(index_expr)
        if index_type and index_type != 'integer':
            error_msg = f"Erro semântico: índice deve ser do tipo integer, mas é do tipo '{index_type}'"
            self.errors.append(error_msg)

    def _analyze_if_statement(self, node):
        """
        Analisa o nó 'if'.
        """
        condition = node[1]
        then_statement = node[2]
        else_statement = node[3]  # Pode ser None

        # Analisa a condição
        cond_type = self._analyze_expression(condition)

        # Verifica se a condição é booleana
        if cond_type and cond_type != 'boolean':
            error_msg = f"Erro semântico: condição deve ser do tipo boolean, mas é do tipo '{cond_type}'"
            self.errors.append(error_msg)

        # Analisa o bloco THEN
        self._analyze_node(then_statement)

        # Analisa o bloco ELSE, se existir
        if else_statement:
            self._analyze_node(else_statement)

    def _analyze_while_statement(self, node):
        """
        Analisa o nó 'while'.
        """
        condition = node[1]
        statement = node[2]

        # Analisa a condição
        cond_type = self._analyze_expression(condition)

        # Verifica se a condição é booleana
        if cond_type and cond_type != 'boolean':
            error_msg = f"Erro semântico: condição deve ser do tipo boolean, mas é do tipo '{cond_type}'"
            self.errors.append(error_msg)

        # Analisa o bloco DO
        self._analyze_node(statement)

    def _analyze_for_statement(self, node):
        """
        Analisa o nó 'for_to' ou 'for_downto'.
        """
        counter = node[1]
        start = node[2]
        end = node[3]
        statement = node[4]

        # Verifica se a variável contadora existe
        counter_symbol = self.lookup_symbol(counter)
        if not counter_symbol:
            error_msg = f"Erro semântico: variável de controle '{counter}' não declarada"
            self.errors.append(error_msg)
        else:
            counter_type = self._get_type_string(counter_symbol['type'])
            if counter_type != 'integer':
                error_msg = f"Erro semântico: variável de controle '{counter}' deve ser do tipo integer, mas é do tipo '{counter_type}'"
                self.errors.append(error_msg)

        # Analisa as expressões
        start_type = self._analyze_expression(start)
        if start_type and start_type != 'integer':
            error_msg = f"Erro semântico: expressão de início deve ser do tipo integer, mas é do tipo '{start_type}'"
            self.errors.append(error_msg)

        end_type = self._analyze_expression(end)
        if end_type and end_type != 'integer':
            error_msg = f"Erro semântico: expressão de fim deve ser do tipo integer, mas é do tipo '{end_type}'"
            self.errors.append(error_msg)

        # Analisa o bloco DO
        self._analyze_node(statement)

    def _analyze_io_statement(self, node):
        """
        Analisa o nó de entrada/saída.
        """
        io_type = node[0]
        params = node[1]

        if io_type in ('read', 'readln'):
            # Para read/readln, verifica se cada parâmetro é uma variável declarada ou acesso a array
            if isinstance(params, list):
                for param in params:
                    if isinstance(param, tuple) and param[0] == 'array_access':
                        self._analyze_array_access(param)
                    elif isinstance(param, str):
                        if not self.lookup_symbol(param):
                            error_msg = f"Erro semântico: variável '{param}' não declarada"
                            self.errors.append(error_msg)
            elif params:
                if isinstance(params, tuple) and params[0] == 'array_access':
                    self._analyze_array_access(params)
                elif isinstance(params, str):
                    if not self.lookup_symbol(params):
                        error_msg = f"Erro semântico: variável '{params}' não declarada"
                        self.errors.append(error_msg)
        else:  # write/writeln
            if isinstance(params, list):
                for param in params:
                    if isinstance(param, tuple) and param[0] == 'STRING_LITERAL':
                        # É uma string literal, não precisa verificar
                        continue
                    elif isinstance(param, str) and (param.startswith("'") or param.startswith('"')):
                        # É uma string literal, não precisa verificar
                        continue
                    else:
                        self._analyze_expression(param)
            elif params:
                if isinstance(params, tuple) and params[0] == 'STRING_LITERAL':
                    # É uma string literal, não precisa verificar
                    pass
                elif isinstance(params, str) and (params.startswith("'") or params.startswith('"')):
                    # É uma string literal, não precisa verificar
                    pass
                else:
                    self._analyze_expression(params)

    def _analyze_operation(self, node):
        """
        Analisa operações aritméticas, relacionais e lógicas.
        """
        operator = node[0]
        operand1 = node[1]
        operand2 = node[2] if len(node) > 2 else None

        # Analisa os operandos
        type1 = self._analyze_expression(operand1)
        type2 = self._analyze_expression(operand2) if operand2 else None

        # Verifica compatibilidade de tipos com o operador
        if operator in ('+', '-', '*', '/'):
            # Operadores aritméticos
            if type1 and type2 and not self._are_numeric_types(type1, type2):
                error_msg = f"Erro semântico: operador '{operator}' requer operandos numéricos, mas recebeu '{type1}' e '{type2}'"
                self.errors.append(error_msg)
                return None

            # Determina o tipo do resultado
            if type1 == 'real' or type2 == 'real':
                return 'real'
            else:
                return 'integer'

        elif operator in ('div', 'mod'):
            # Divisão inteira e módulo
            if type1 and type1 != 'integer':
                error_msg = f"Erro semântico: operador '{operator}' requer primeiro operando do tipo integer, mas recebeu '{type1}'"
                self.errors.append(error_msg)

            if type2 and type2 != 'integer':
                error_msg = f"Erro semântico: operador '{operator}' requer segundo operando do tipo integer, mas recebeu '{type2}'"
                self.errors.append(error_msg)

            return 'integer'

        elif operator in ('=', '<>', '<', '>', '<=', '>='):
            # Operadores relacionais
            if type1 and type2 and not self._are_comparable_types(type1, type2):
                error_msg = f"Erro semântico: não é possível comparar tipos '{type1}' e '{type2}' com operador '{operator}'"
                self.errors.append(error_msg)

            return 'boolean'

        elif operator in ('and', 'or'):
            # Operadores lógicos
            if type1 and type1 != 'boolean':
                error_msg = f"Erro semântico: operador '{operator}' requer primeiro operando do tipo boolean, mas recebeu '{type1}'"
                self.errors.append(error_msg)

            if type2 and type2 != 'boolean':
                error_msg = f"Erro semântico: operador '{operator}' requer segundo operando do tipo boolean, mas recebeu '{type2}'"
                self.errors.append(error_msg)

            return 'boolean'

        elif operator == 'not':
            # Operador de negação lógica
            if type1 and type1 != 'boolean':
                error_msg = f"Erro semântico: operador 'not' requer operando do tipo boolean, mas recebeu '{type1}'"
                self.errors.append(error_msg)

            return 'boolean'

        return None

    def _analyze_expression(self, expression):
        """
        Analisa uma expressão e determina seu tipo.
        """
        # Se é uma operação
        if isinstance(expression, tuple):
            if expression[0] == 'array_access':
                # É um acesso a array ou string
                self._analyze_array_access(expression)
                name = expression[1]
                symbol = self.lookup_symbol(name)
                if symbol:
                    if isinstance(symbol['type'], tuple) and symbol['type'][0] == 'array_type':
                        return self._get_type_string(symbol['type'])
                    elif symbol['type'] == 'string':
                        return 'char'  # Acesso a caractere de string
                return 'integer'  # fallback
            elif expression[0] == 'function_call':
                # É uma chamada de função
                func_name = expression[1]
                if func_name == 'length':
                    return 'integer'
                else:
                    # Função definida pelo usuário
                    func_symbol = self.lookup_symbol(func_name)
                    if func_symbol and func_symbol['kind'] == 'function':
                        return func_symbol['type']
                    return None
            elif expression[0] == 'char_literal':
                return 'char'
            else:
                # Chama _analyze_operation para analisar a operação
                return self._analyze_operation(expression)

        # Se é um identificador (variável)
        elif isinstance(expression, str) and expression[0].isalpha():
            symbol = self.lookup_symbol(expression)
            if not symbol:
                error_msg = f"Erro semântico: variável '{expression}' não declarada"
                self.errors.append(error_msg)
                return None
            return self._get_type_string(symbol['type'])

        # Se é um inteiro
        elif isinstance(expression, int):
            return 'integer'

        # Se é um número real
        elif isinstance(expression, float):
            return 'real'

        # Se é uma string
        if isinstance(expression, str) and (expression.startswith("'") or expression.startswith('"')):
            return 'string'

        # Se é um booleano
        elif expression in ('true', 'false'):
            return 'boolean'

        # Tipo desconhecido
        return None

    def _is_compatible_type(self, target_type, source_type):
        """
        Verifica se um tipo é compatível com outro para atribuição.
        """
        # Tipos iguais são sempre compatíveis
        if target_type == source_type:
            return True

        # Conversões permitidas:
        # - integer para real
        if target_type == 'real' and source_type == 'integer':
            return True

        # - char para string (em alguns contextos)
        if target_type == 'string' and source_type == 'char':
            return True

        return False

    def _are_numeric_types(self, type1, type2):
        """
        Verifica se ambos os tipos são numéricos.
        """
        return (type1 in ('integer', 'real')) and (type2 in ('integer', 'real'))

    def _are_comparable_types(self, type1, type2):
        """
        Verifica se dois tipos podem ser comparados.
        """
        # Tipos iguais sempre podem ser comparados
        if type1 == type2:
            return True

        # Tipos numéricos podem ser comparados entre si
        if (type1 in ('integer', 'real')) and (type2 in ('integer', 'real')):
            return True

        # Char e string podem ser comparados
        if (type1 in ('char', 'string')) and (type2 in ('char', 'string')):
            return True

        return False