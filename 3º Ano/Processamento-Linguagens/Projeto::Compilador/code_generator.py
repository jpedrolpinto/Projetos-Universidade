class CodeGenerator:
    """
    Classe para o gerador de código para a EWVM.
    """

    def __init__(self):
        """
        Inicializa o gerador de código.
        """
        self.code = []
        self.label_counter = 0

        # Mapear variáveis para posições na memória global
        self.variables = {}
        self.var_counter = 0

        # Mapear tipos das variáveis para distinguir strings de arrays
        self.variable_types = {}

        # Para rastrear o escopo atual (global ou local)
        self.current_scope = 'global'

        # Para rastrear funções
        self.functions = {}
        self.current_function = None

    def generate(self, node):
        """
        Gera código a partir da árvore sintática.
        """
        self.code = []
        self.variables = {}  # Reset variables
        self.variable_types = {}  # Reset types
        self.var_counter = 0  # Reset counter
        self._generate_code(node)
        return self.code

    def _get_type_string(self, type_info):
        """
        Extrai uma string de tipo de uma informação de tipo que pode ser string ou tupla.
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

    def _generate_code(self, node):
        """
        Gera código para um nó da árvore sintática.
        """
        if not node:
            return None

        # Se o nó for um tuple, verifica o tipo de nó e chama a função apropriada
        if isinstance(node, tuple):
            node_type = node[0]

            # Diferentes tipos de nós da AST
            if node_type == 'program':
                return self._generate_program(node)
            elif node_type == 'function_declaration':
                return self._generate_function_declaration(node)
            elif node_type == 'block':
                return self._generate_block(node)
            elif node_type == 'declarations':
                return self._generate_declarations(node)
            elif node_type == 'declaration':
                return self._generate_declaration(node)
            elif node_type == 'compound':
                return self._generate_compound_statement(node)
            elif node_type == 'assign':
                return self._generate_assignment(node)
            elif node_type == 'function_return':
                return self._generate_function_return(node)
            elif node_type == 'if':
                return self._generate_if_statement(node)
            elif node_type == 'while':
                return self._generate_while_statement(node)
            elif node_type == 'for_to':
                return self._generate_for_to_statement(node)
            elif node_type == 'for_downto':
                return self._generate_for_downto_statement(node)
            elif node_type in ('writeln', 'write', 'read', 'readln'):
                return self._generate_io_statement(node)
            elif node_type in ('+', '-', '*', '/', 'div', 'mod', '=', '<>', '<', '>', '<=', '>=', 'and', 'or', 'not'):
                return self._generate_operation(node)
            elif node_type == 'array_access':
                return self._generate_array_access(node)
            elif node_type == 'function_call':
                return self._generate_function_call(node)
            elif node_type == 'char_literal':
                return self._generate_char_literal(node)
            else:
                # Para outros tipos de nós, gera código recursivamente para os filhos
                for child in node[1:]:
                    self._generate_code(child)
                return None

        # Se o nó for uma lista, gera código para cada elemento
        elif isinstance(node, list):
            for item in node:
                self._generate_code(item)
            return None

        # Se o nó for um valor literal ou um identificador
        elif isinstance(node, float):
            self.code.append(f"PUSHF {node}")
        elif isinstance(node, int):
            self.code.append(f"PUSHI {node}")
        else:
            return node

    def _new_label(self):
        """
        Cria um novo rótulo (label) para saltos.
        """
        label = f"L{self.label_counter}"
        self.label_counter += 1
        return label

    def _generate_program(self, node):
        """
        Gera código para o nó 'program'.
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

        self.code.append(f"// Programa: {program_name}")
        self.code.append("")

        # Gera código para as funções primeiro
        for function in functions:
            self._generate_code(function)
            self.code.append("")

        self._generate_code(block)
        self.code.append("")
        self.code.append("STOP")

    def _generate_function_declaration(self, node):
        """Gera código para declaração de função com tratamento correto de escopo"""
        func_name = node[1]
        parameters = node[2]
        block = node[4]

        func_label = f"FUNC_{func_name}"
        self.code.append(f"// Função: {func_name}")
        self.code.append(f"{func_label}:")

        # Salva contexto
        old_function = self.current_function
        old_variables = self.variables.copy()
        old_types = self.variable_types.copy()  # Salva tipos
        old_counter = self.var_counter

        self.current_function = func_name
        self.functions[func_name] = func_label
        self.variables = {}  # Novo scope para parâmetros
        self.variable_types = {}  # Novo scope para tipos
        self.var_counter = 0

        # Processa parâmetros
        param_offset = 0
        for param in parameters:
            param_names = param[1]
            param_type = param[2]
            for param_name in param_names:
                self.variables[param_name] = param_offset
                self.variable_types[param_name] = param_type  # Registra tipo
                param_offset += 1

        # Gera código do bloco
        self._generate_code(block)
        self.code.append("RETURN")

        # Restaura contexto
        self.current_function = old_function
        self.variables = old_variables
        self.variable_types = old_types  # Restaura tipos
        self.var_counter = old_counter

    def _generate_function_return(self, node):
        """
        Gera código para atribuição de valor de retorno.
        """
        # Formato: ('function_return', function_name, expression)
        func_name = node[1]
        expression = node[2]

        # Gera código para a expressão
        self._generate_expression_value(expression)

        # Para simplificar, armazena o resultado numa posição específica
        # Numa implementação completa, usaria a pilha de chamadas
        self.code.append("// Valor de retorno da função")
        self.code.append("STOREG -1")  # Posição especial para retorno

    def _generate_char_literal(self, node):
        """
        Gera código para literal de caractere.
        """
        # Formato: ('char_literal', char_value)
        char_value = node[1]
        ascii_value = ord(char_value)  # Converte para código ASCII
        self.code.append(f"PUSHI {ascii_value}")

    def  _generate_block(self, node):
        """
        Gera código para o nó 'block'.
        """
        # Formato: ('block', declarations, compound_statement)
        declarations = node[1]
        compound_statement = node[2]

        # Gera código para as declarações (variáveis)
        self._generate_code(declarations)

        # Marca o início do programa (apenas se não estiver em função)
        if self.current_function is None:
            self.code.append("START")
            self.code.append("")

        # Gera código para o bloco de instruções
        self._generate_code(compound_statement)

    def _generate_declarations(self, node):
        """
        Gera código para o nó 'declarations'.
        """
        # Formato: ('declarations', [declaration1, declaration2, ...])
        declarations = node[1]

        if not declarations:
            return

        # Gera código para cada declaração
        if isinstance(declarations, list):
            for declaration in declarations:
                if declaration:  # Verifica se não é None
                    self._generate_code(declaration)
        else:
            if declarations:  # Verifica se não é None
                self._generate_code(declarations)

    def _generate_declaration(self, node):
        """
        Gera código para declaração registrando tipos
        """
        var_names = node[1]
        var_type = node[2]

        for var_name in var_names:
            var_index = self.var_counter

            self.code.append(f"// Declaração da variável {var_name}")

            # registra o tipo de variavel
            self.variable_types[var_name] = var_type

            if isinstance(var_type, tuple) and var_type[0] == 'array_type':
                # Array declaration
                array_range = var_type[1]
                min_val, max_val = array_range
                array_size = max_val - min_val + 1

                self.code.append(f"PUSHI {array_size}")
                self.code.append("ALLOCN")
                self.code.append(f"STOREG {var_index}")
            else:
                # Variable declaration
                type_str = self._get_type_string(var_type)
                if type_str == 'real':
                    self.code.append("PUSHF 0.0")
                elif type_str == 'boolean':
                    self.code.append("PUSHI 0")
                elif type_str == 'char':
                    self.code.append("PUSHI 0")
                elif type_str == 'string':
                    self.code.append("PUSHS \"\"")
                else:  # integer
                    self.code.append("PUSHI 0") # valor inicial

                self.code.append(f"STOREG {var_index}") # posiçao na memoria

            self.variables[var_name] = var_index # Mapeia var -> posiçao memoria
            self.var_counter += 1

    def _generate_compound_statement(self, node):
        """
        Gera código para o nó 'compound'.
        """
        # Formato: ('compound', [statement1, statement2, ...])
        statements = node[1]

        # Gera código para cada instrução
        for statement in statements:
            if statement:  # Verifica se não é None
                self._generate_code(statement)

    def _generate_assignment(self, node):
        # Formato: ('assign', variable_name_or_array_access, expression)
        var_or_array = node[1]
        expression = node[2]

        # Verifica se é atribuição a array/string ou variável simples
        if isinstance(var_or_array, tuple) and var_or_array[0] == 'array_access':
            # É uma atribuição a array ou string: array[index] := expr ou string[index] := expr
            array_name = var_or_array[1]
            index_expr = var_or_array[2]

            if array_name not in self.variables:
                raise Exception(f"Array/String não declarado: {array_name}")

            # decisao baseada no tipo de variavel
            var_type = self.variable_types.get(array_name, 'unknown')

            if var_type == 'string':
                raise Exception(f"Erro: Não é possível modificar caracteres de string na EWVM - strings são imutáveis")

            else:
                # STOREN espera na stack: [endereço (fundo), índice (meio), valor (topo)]

                # 1. Empilha endereço do array (vai para o fundo)
                self.code.append(f"PUSHG {self.variables[array_name]}")

                # 2. Empilha índice ajustado (vai para o meio)
                self._generate_expression_value(index_expr)
                self.code.append("PUSHI 1")
                self.code.append("SUB")  # ajusta para base 0

                # 3. Empilha valor da expressão (vai para o topo)
                self._generate_expression_value(expression)

                # 4. Executa STOREN
                self.code.append("STOREN")

        else:
            # É uma atribuição simples: var := expr
            var_name = var_or_array

            if var_name not in self.variables:
                raise Exception(f"Variável não declarada: {var_name}")

            # Gera código para a expressão ANTES de STOREG
            self._generate_expression_value(expression)

            # Gera a instrução para armazenar o valor na variável
            var_index = self.variables[var_name]
            self.code.append(f"STOREG {var_index}")

    def _generate_expression_value(self, expression):
        """
        Gera código para uma expressão que deve deixar um valor na pilha.
        """
        if isinstance(expression, str):
            # É um identificador de variável
            if expression in self.variables:
                var_index = self.variables[expression]
                self.code.append(f"PUSHG {var_index}")
            elif expression in ('true', 'false'):
                value = 1 if expression == 'true' else 0
                self.code.append(f"PUSHI {value}")
            elif expression.startswith("'") or expression.startswith('"'):
                if expression.startswith("'") and expression.endswith("'"):
                    expression = expression[1:-1]
                self.code.append(f"PUSHS \"{expression}\"")
            else:
                raise Exception(f"Identificador não declarado: {expression}")
        elif isinstance(expression, int):
            self.code.append(f"PUSHI {expression}")
        elif isinstance(expression, float):
            self.code.append(f"PUSHF {expression}")
        elif isinstance(expression, tuple):
            if expression[0] == 'array_access':
                self._generate_array_access(expression)
            elif expression[0] == 'function_call':
                self._generate_function_call(expression)
            elif expression[0] == 'char_literal':
                self._generate_char_literal(expression)
            else:
                # É uma operação ou expressão complexa
                self._generate_code(expression)
        else:
            # Para outros tipos, usa o método genérico
            self._generate_code(expression)

    def _generate_array_access(self, node):
        # Formato: ('array_access', name, index_expr)
        name = node[1]
        index_expr = node[2]

        if name not in self.variables:
            raise Exception(f"Variável não declarada: {name}")

        # decisao baseada no tipo real da variavel
        var_type = self.variable_types.get(name, 'unknown')

        if var_type == 'string':
            # Baseado no output esperado: string primeiro, depois índice

            # 1. Empilha string reference PRIMEIRO
            self.code.append(f"PUSHG {self.variables[name]}")

            # 2. Empilha índice ajustado DEPOIS
            self._generate_expression_value(index_expr)
            self.code.append("PUSHI 1")
            self.code.append("SUB")  # ajusta para base 0 da EWVM

            # 3. Usa CHARAT para acessar caractere da string
            self.code.append("CHARAT")

        else:
            # LOADN espera na stack: [endereço (fundo), índice (topo)]

            # 1. Empilha endereço do array PRIMEIRO (vai para o fundo)
            self.code.append(f"PUSHG {self.variables[name]}")

            # 2. Empilha índice ajustado DEPOIS (vai para o topo)
            self._generate_expression_value(index_expr)
            self.code.append("PUSHI 1")
            self.code.append("SUB")  # ajusta para base 0

            # 3. Executa LOADN
            self.code.append("LOADN")

    def _generate_if_statement(self, node):
        """
        Gera código para o nó 'if'.
        """
        # Formato: ('if', condition, then_statement, else_statement)
        condition = node[1]
        then_statement = node[2]
        else_statement = node[3]  # Pode ser None

        # Gera rótulos para os saltos
        else_label = self._new_label()
        end_label = self._new_label()

        # Usar _generate_expression_value para garantir valor na pilha
        self._generate_expression_value(condition)

        # Instrução de salto condicional
        self.code.append(f"JZ {else_label}")

        # Gera código para o bloco THEN
        self._generate_code(then_statement)

        # Se houver um bloco ELSE, salta para o fim
        if else_statement:
            self.code.append(f"JUMP {end_label}")

        # Rótulo para o bloco ELSE
        self.code.append(f"{else_label}:")

        # Gera código para o bloco ELSE, se existir
        if else_statement:
            self._generate_code(else_statement)
            self.code.append(f"{end_label}:")

    def _generate_while_statement(self, node):
        """
        Gera código para o nó 'while'.
        """
        # Formato: ('while', condition, statement)
        condition = node[1]
        statement = node[2]

        start_label = self._new_label()
        end_label = self._new_label()

        self.code.append("// Início do ciclo while")
        self.code.append(f"{start_label}:")

        # Usar _generate_expression_value para garantir valor na pilha
        self._generate_expression_value(condition)

        self.code.append(f"JZ {end_label}")
        self._generate_code(statement)
        self.code.append(f"JUMP {start_label}")
        self.code.append(f"{end_label}:")
        self.code.append("// Fim do ciclo while")

    def _generate_for_to_statement(self, node):
        """
        Gera código para o nó 'for_to'.
        """
        # Formato: ('for_to', counter, start, end, statement)
        counter = node[1]
        start = node[2]
        end = node[3]
        statement = node[4]

        if counter not in self.variables:
            raise Exception(f"Variável não declarada: {counter}")

        counter_index = self.variables[counter]
        loop_start = self._new_label()
        loop_end = self._new_label()

        self.code.append("// Início do ciclo for")

        # Inicializa o contador
        self._generate_expression_value(start)
        self.code.append(f"STOREG {counter_index}")

        # Início do loop
        self.code.append(f"{loop_start}:")

        # Testar se i <= n
        self.code.append(f"PUSHG {counter_index}")  # empilha i
        self._generate_expression_value(end)  # empilha n
        self.code.append("INFEQ")  # i <= n (resultado: 1 se verdadeiro, 0 se falso)
        self.code.append(f"JZ {loop_end}")  # se i <= n é 0 (falso), sair do loop

        # Corpo do loop
        self._generate_code(statement)

        # Incrementa o contador
        self.code.append(f"PUSHG {counter_index}")
        self.code.append("PUSHI 1")
        self.code.append("ADD")
        self.code.append(f"STOREG {counter_index}")

        self.code.append(f"JUMP {loop_start}")
        self.code.append(f"{loop_end}:")
        self.code.append("// Fim do ciclo for")

    def _generate_for_downto_statement(self, node):
        """
        Gera código para o nó 'for_downto'.
        """
        counter = node[1]
        start = node[2]
        end = node[3]
        statement = node[4]

        if counter not in self.variables:
            raise Exception(f"Variável não declarada: {counter}")

        counter_index = self.variables[counter]
        loop_start = self._new_label()
        loop_end = self._new_label()

        self.code.append("// Início do ciclo for downto")

        # Inicializa o contador
        self._generate_expression_value(start)
        self.code.append(f"STOREG {counter_index}")

        # Início do loop
        self.code.append(f"{loop_start}:")

        # Testar se i >= end
        self.code.append(f"PUSHG {counter_index}")  # empilha i
        self._generate_expression_value(end)  # empilha end
        self.code.append("SUPEQ")  # i >= end (resultado: 1 se verdadeiro, 0 se falso)
        self.code.append(f"JZ {loop_end}")  # se i >= end é 0 (falso), sair do loop

        # Corpo do loop
        self._generate_code(statement)

        # Decrementa o contador
        self.code.append(f"PUSHG {counter_index}")
        self.code.append("PUSHI 1")
        self.code.append("SUB")
        self.code.append(f"STOREG {counter_index}")

        self.code.append(f"JUMP {loop_start}")
        self.code.append(f"{loop_end}:")
        self.code.append("// Fim do ciclo for downto")

    def _generate_io_statement(self, node):
        io_type = node[0]
        params = node[1] if len(node) > 1 else []

        if io_type in ('read', 'readln'):
            # Para read/readln
            if isinstance(params, list):
                for param in params:
                    if isinstance(param, tuple) and param[0] == 'array_access':
                        array_name = param[1]
                        index_expr = param[2]

                        if array_name not in self.variables:
                            raise Exception(f"Array/String não declarado: {array_name}")

                        # decisao baseada no tipo
                        var_type = self.variable_types.get(array_name, 'unknown')

                        if var_type == 'string':
                            raise Exception(
                                f"Erro: Não é possível ler para string[index] - strings são imutáveis na EWVM")
                        else:
                            self.code.append(f"PUSHG {self.variables[array_name]}")
                            self._generate_expression_value(index_expr)
                            self.code.append("PUSHI 1")
                            self.code.append("SUB")
                            self.code.append("READ")
                            self.code.append("ATOI")
                            self.code.append("STOREN")

                    elif isinstance(param, str):
                        if param not in self.variables:
                            raise Exception(f"Variável não declarada: {param}")

                        var_index = self.variables[param]
                        var_type = self.variable_types.get(param, 'unknown')

                        self.code.append("READ")
                        # Converte para inteiro se necessário
                        if var_type == 'integer':
                            self.code.append("ATOI")
                        elif var_type == 'real':
                            self.code.append("ATOF")
                        # Para strings, não converte
                        self.code.append(f"STOREG {var_index}")

            elif params:
                # Parâmetro único
                if isinstance(params, tuple) and params[0] == 'array_access':
                    array_name = params[1]
                    index_expr = params[2]

                    var_type = self.variable_types.get(array_name, 'unknown')

                    if var_type == 'string':
                        raise Exception(f"Erro: Não é possível ler para string[index] - strings são imutáveis na EWVM")
                    else:
                        self.code.append(f"PUSHG {self.variables[array_name]}")
                        self._generate_expression_value(index_expr)
                        self.code.append("PUSHI 1")
                        self.code.append("SUB")
                        self.code.append("READ")
                        self.code.append("ATOI")
                        self.code.append("STOREN")
                elif isinstance(params, str):
                    if params not in self.variables:
                        raise Exception(f"Variável não declarada: {params}")
                    var_index = self.variables[params]
                    var_type = self.variable_types.get(params, 'unknown')

                    self.code.append("READ")
                    # Converte para inteiro se necessário
                    if var_type == 'integer':
                        self.code.append("ATOI")
                    elif var_type == 'real':
                        self.code.append("ATOF")
                    # Para strings, não converte
                    self.code.append(f"STOREG {var_index}")

        else:  # write/writeln
            if isinstance(params, list):
                for param in params:
                    if isinstance(param, tuple) and param[0] == 'STRING_LITERAL':
                        string_content = param[1]
                        if string_content.startswith("'") and string_content.endswith("'"):
                            string_content = string_content[1:-1]
                        self.code.append(f"PUSHS \"{string_content}\"")
                        self.code.append("WRITES")
                    elif isinstance(param, str) and (param.startswith("'") or param.startswith('"')):
                        if param.startswith("'") and param.endswith("'"):
                            param = param[1:-1]
                        self.code.append(f"PUSHS \"{param}\"")
                        self.code.append("WRITES")
                    else:
                        self._generate_expression_value(param)
                        # Detecta se é string ou inteiro baseado no tipo
                        if isinstance(param, str) and param in self.variable_types:
                            var_type = self.variable_types[param]
                            if var_type == 'string':
                                self.code.append("WRITES")
                            else:
                                self.code.append("WRITEI")
                        else:
                            self.code.append("WRITEI")

            elif params:
                if isinstance(params, tuple) and params[0] == 'STRING_LITERAL':
                    string_content = params[1]
                    if string_content.startswith("'") and string_content.endswith("'"):
                        string_content = string_content[1:-1]
                    self.code.append(f"PUSHS \"{string_content}\"")
                    self.code.append("WRITES")
                elif isinstance(params, str) and (params.startswith("'") or params.startswith('"')):
                    if params.startswith("'") and params.endswith("'"):
                        params = params[1:-1]
                    self.code.append(f"PUSHS \"{params}\"")
                    self.code.append("WRITES")
                else:
                    self._generate_expression_value(params)
                    self.code.append("WRITEI")

            # Para writeln, adiciona nova linha
            if io_type == 'writeln':
                self.code.append("WRITELN")

    def _generate_function_call(self, node):
        """
        Gera código para chamadas de função built-in e definidas pelo usuário.
        """
        # Formato: ('function_call', function_name, [arg1, arg2, ...])
        function_name = node[1]
        arguments = node[2]

        if function_name == 'length':
            # Função built-in length
            if len(arguments) == 1:
                arg = arguments[0]

                # Se for uma string literal, calcular o comprimento
                if isinstance(arg, str) and (arg.startswith("'") or arg.startswith('"')):
                    # String literal - calcular comprimento em tempo de compilação
                    string_content = arg[1:-1] if arg.startswith("'") else arg
                    length = len(string_content)
                    self.code.append(f"PUSHI {length}")
                else:
                    # Para variável string, garante que PUSHG carregue a referência da string
                    # De seguida, usa STRLEN para calcular o comprimento
                    if isinstance(arg, str) and arg in self.variables:
                        self.code.append(f"PUSHG {self.variables[arg]}")
                        self.code.append("STRLEN")
                    else:
                        # Expressão mais complexa
                        self._generate_expression_value(arg)
                        self.code.append("STRLEN")
            else:
                raise Exception("Função length() espera exatamente 1 argumento")
        else:
            # Função definida pelo usuário
            if function_name not in self.functions:
                raise Exception(f"Função não declarada: {function_name}")

            # Gera código para os argumentos
            for arg in arguments:
                self._generate_expression_value(arg)

            # Chama a função
            func_label = self.functions[function_name]
            self.code.append(f"PUSHA {func_label}")
            self.code.append("CALL")

            # O resultado fica na posição -1 (simplificado)
            self.code.append("PUSHG -1")

    def _generate_operation(self, node):
        """
        Gera código para operações aritméticas, relacionais e lógicas.
        """
        operator = node[0]
        operand1 = node[1]
        operand2 = node[2] if len(node) > 2 else None

        # Para operações binárias
        if operand2 is not None:
            self._generate_expression_value(operand1)
            self._generate_expression_value(operand2)
        else:
            # Para operações unárias
            self._generate_expression_value(operand1)

        # Mapeia operadores
        op_map = {
            '+': 'ADD', '-': 'SUB', '*': 'MUL', '/': 'DIV', 'div': 'DIV', 'mod': 'MOD',
            '=': 'EQUAL', '<>': 'EQUAL\nNOT', '<': 'INF', '>': 'SUP',
            '<=': 'INFEQ', '>=': 'SUPEQ', 'and': 'AND', 'or': 'OR', 'not': 'NOT'
        }

        if operator == '<>':
            self.code.append('EQUAL')
            self.code.append('NOT')
        else:
            self.code.append(op_map[operator])

    def _generate_factor(self, node):
        """
        Gera código para um fator (identificador ou valor literal).
        """
        if isinstance(node, tuple):
            if node[0] == 'array_access':
                self._generate_array_access(node)
            elif node[0] == 'function_call':
                self._generate_function_call(node)
            elif node[0] == 'char_literal':
                self._generate_char_literal(node)
            else:
                # Para outros tuples (operações), gera código recursivamente
                self._generate_code(node)
        elif isinstance(node, str):
            if node in self.variables:
                var_index = self.variables[node]
                self.code.append(f"PUSHG {var_index}")
            elif node.startswith("'") or node.startswith('"'):
                if node.startswith("'") and node.endswith("'"):
                    node = node[1:-1]
                self.code.append(f"PUSHS \"{node}\"")
            elif node in ('true', 'false'):
                value = 1 if node == 'true' else 0
                self.code.append(f"PUSHI {value}")
            else:
                raise Exception(f"Identificador não declarado: {node}")
        elif isinstance(node, int):
            self.code.append(f"PUSHI {node}")
        elif isinstance(node, float):
            self.code.append(f"PUSHF {node}")
        elif isinstance(node, list):
            # Para listas, processa cada elemento
            for item in node:
                self._generate_factor(item)
        else:
            # Para nós complexos, gera código recursivamente
            self._generate_code(node)