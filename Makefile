# DSA Visualizer Compiler - build from project root
# Source files live in src/; generated parser/lexer and binaries in root

all: dsa_compiler json_to_js cli_visualizer

parser.tab.c parser.tab.h: src/parser.y
	bison -d src/parser.y

lex.yy.c: src/lexer.l
	flex -o lex.yy.c src/lexer.l

dsa_compiler: parser.tab.c lex.yy.c src/ast.c src/ir.c src/codegen.c src/main.c
	gcc -std=c11 -Wall -Wextra -Werror -Isrc -o dsa_compiler parser.tab.c lex.yy.c src/ast.c src/ir.c src/codegen.c src/main.c -lfl

json_to_js: src/json_to_js.c
	gcc -std=c11 -Wall -Wextra -o json_to_js src/json_to_js.c

cli_visualizer: src/cli_visualizer.c
	gcc -std=c11 -Wall -Wextra -o cli_visualizer src/cli_visualizer.c

clean:
	rm -f parser.tab.c parser.tab.h lex.yy.c
	rm -f dsa_compiler dsa_compiler.exe json_to_js json_to_js.exe cli_visualizer cli_visualizer.exe
	rm -f output.json web/visualizer.js
