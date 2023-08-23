root  = ./..
obj   = ./obj

in = ./input
out = ./output

csrpg = ${root}/bin/csrpg
csr-1 = ${obj}/translated-pascal.csr
csr-2 = ${obj}/translated-pascal-notree.csr
lex   = pascal.l

FLAGS = -O2




.PHONY: all
all: ${obj}/program ${obj}/program-notree

# .PHONY: run
# run: ${obj}/program
# 	${obj}/program -stats <"${in}/mf.p"




${obj}:
	mkdir -p ${obj}




# Create program
${obj}/program: ${obj}/parser.o ${obj}/lexer.o ${obj}/main.o | ${obj}
	gcc ${filter %.o, $^} ${FLAGS} -lfl -o "$@"
${obj}/program-notree: ${obj}/parser-notree.o ${obj}/lexer.o ${obj}/main.o | ${obj}
	gcc ${filter %.o, $^} ${FLAGS} -lfl -o "$@"

${obj}/main.o: main.c | ${obj}
	gcc ${filter %.c, $^} ${FLAGS} -c -o "$@"



# Create parser
${obj}/parser.o: ${obj}/parser.c | ${obj}
	gcc "$(filter %.c, $^)" ${FLAGS} -c -o "$@"
${obj}/parser.c: ${csrpg} ${csr-1} | ${obj}
	${csrpg} -i "${csr-1}" -o "$@" -h "${obj}/parser.h" -t "${obj}/tokens.h"
	
${obj}/parser-notree.o: ${obj}/parser-notree.c | ${obj}
	gcc "$(filter %.c, $^)" ${FLAGS} -c -o "$@"
${obj}/parser-notree.c: ${csrpg} ${csr-2} | ${obj}
	${csrpg} -i "${csr-2}" -o "$@" -h "${obj}/parser.h" -t "${obj}/tokens.h"

${csrpg}:
	make -C "${root}"


# Create lexer
${obj}/lexer.o: ${obj}/lex.yy.c | ${obj}
	gcc "${obj}/lex.yy.c" ${FLAGS} -lfl -c -o "$@"
${obj}/lex.yy.c: ${lex} | ${obj}
	flex -o "${obj}/lex.yy.c" ${lex}


# Translate csr
.PHONY: translate
translate: ${csr-1} ${csr-2}
	
${csr-1}: ${obj}/csr-translator | ${obj}
	${obj}/csr-translator <"pascal.csr+" >"$@"
${csr-2}: ${obj}/csr-translator | ${obj}
	${obj}/csr-translator --no-tree <"pascal.csr+" >"$@"

${obj}/csr-translator: csr-translator.cpp | ${obj}
	g++ $(filter %.cpp, $^) -o "$@"




.PHONY: clean
clean:
	rm -rf "${obj}"
