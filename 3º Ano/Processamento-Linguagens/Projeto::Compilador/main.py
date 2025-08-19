"""
Módulo principal do Compilador.
"""
import os
import argparse

from lexer import PascalLexer
from parser import parse
from semantic import SemanticAnalyzer
from code_generator import CodeGenerator


def compile_file(file_path, output_path=None, debug=False):
    """
    Compila um arquivo de código fonte.

    Args:
        file_path (str): Caminho para o arquivo de código fonte.
        output_path (str, opcional): Caminho para o arquivo de saída.
        debug (bool): Se True, imprime informações detalhadas durante a compilação.

    Returns:
        bool: True se a compilação foi bem-sucedida, False caso contrário.
    """
    try:
        # Lê o código fonte
        with open(file_path, 'r') as file:
            source_code = file.read()

        if debug:
            print(f"Arquivo lido: {file_path}")

        # Define o caminho de saída padrão se não for fornecido
        if not output_path:
            output_path = os.path.splitext(file_path)[0] + '.vm'

        # Cria as instâncias dos componentes do compilador
        lexer = PascalLexer()
        semantic_analyzer = SemanticAnalyzer()
        code_generator = CodeGenerator()

        if debug:
            print("\nAnálise Léxica:")
            print("--------------")
            lexer_instance = lexer.build()
            lexer_instance.input(source_code)
            for token in lexer_instance:
                print(f"Linha {token.lineno}: {token.type}('{token.value}')")

        # Análise sintática
        ast = parse(source_code, lexer)

        if debug:
            print("\nÁrvore Sintática:")
            print(ast)

        # Análise semântica
        if not semantic_analyzer.analyze(ast):
            print("\nErros semânticos encontrados:")
            for error in semantic_analyzer.errors:
                print(f"  - {error}")
            print("\nCompilação abortada devido a erros semânticos.")
            return False

        if debug:
            print("\nAnálise Semântica bem-sucedida!")

        # Geração de código
        code = code_generator.generate(ast)

        # Escreve o código gerado no arquivo de saída
        with open(output_path, 'w') as file:
            for line in code:
                file.write(line + '\n')

        print(f"\nCompilação bem-sucedida! Código gerado em: {output_path}")

        if debug:
            print("\nCódigo gerado:")
            print("--------------")
            for line in code:
                print(line)

        return True

    except FileNotFoundError:
        print(f"Erro: Arquivo '{file_path}' não encontrado.")
        return False
    except Exception as e:
        print(f"Erro durante a compilação: {e}")
        import traceback
        traceback.print_exc()
        return False


def main():
    """
    Função principal que processa os argumentos da linha de comando
    e inicia o processo de compilação.
    """
    # Configura o parser de argumentos
    parser = argparse.ArgumentParser(description='Compilador Pascal Completo')
    parser.add_argument('file', help='Arquivo de código fonte')
    parser.add_argument('-o', '--output', help='Arquivo de saída')
    parser.add_argument('-d', '--debug', action='store_true', help='Modo de depuração')

    # Processa os argumentos
    args = parser.parse_args()

    # Compila o arquivo
    compile_file(args.file, args.output, args.debug)


if __name__ == "__main__":
    main()