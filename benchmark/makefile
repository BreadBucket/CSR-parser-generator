root  = ./..
obj   = ./obj

in = ./input
out = ./output

csrpg = ${root}/bin/csrpg
csr   = ${obj}/translated-pascal.csr
lex   = pascal.l

FLAGS = -O2




.PHONY: all
all: ${obj}/program

.PHONY: run
run: ${obj}/program
	${obj}/program <"${in}/mf.p"


${obj}:
	mkdir -p ${obj}





# Create program
${obj}/program: ${obj}/parser.o ${obj}/lexer.o ${obj}/main.o | ${obj}
	gcc ${filter %.o, $^} ${FLAGS} -lfl -o "$@"

${obj}/main.o: main.c | ${obj}
	gcc ${filter %.c, $^} ${FLAGS} -c -o "$@"



# Create parser
${obj}/parser.o: ${obj}/parser.c | ${obj}
	gcc "$(filter %.c, $^)" ${FLAGS} -c -o "$@"

${obj}/parser.c: ${csrpg} ${csr} | ${obj}
	${csrpg} -i "${csr}" -o "${obj}/parser.c" -h "${obj}/parser.h" -t "${obj}/tokens.h"

${csrpg}:
	make -C "${root}"


# Create lexer
${obj}/lexer.o: ${obj}/lex.yy.c | ${obj}
	gcc "${obj}/lex.yy.c" ${FLAGS} -lfl -c -o "$@"

${obj}/lex.yy.c: ${lex} | ${obj}
	flex -o "${obj}/lex.yy.c" ${lex}


# Translate csr
${csr}: ${obj}/csr-translator | ${obj}
	${obj}/csr-translator <"pascal.csr+" >"$@"

${obj}/csr-translator: csr-translator.cpp | ${obj}
	g++ $(filter %.cpp, $^) -o "$@"




.PHONY: clean
clean:
	rm -rf "${obj}"



# .PHONY: all
# all: ${bin}/pascal

# .PHONY: run
# run: ${bin}/pascal
# 	cat "${in}/mf.p" | ./lexer | ${bin}/pascal -stat

# # .PHONY: translate
# # translate: ${bin}/translate
# # 	${bin}/translate <"old-pascal.csr" >"pascal.csr"

# .PHONY: clean
# clean:
# 	rm -rf "${obj}" "${bin}/translate" "${bin}/pascal"
# 	rm -f "${out}/mf.txt" "${out}/tangle.txt" "${out}/weave.txt" "${out}/tex.txt"
# 	@echo "Project cleaned."

# .PHONY: getProg
# getProg:
# 	@echo "${bin}/pascal"



# # Directories
# ${obj} ${bin}:
# 	mkdir -p "$@"


# # Program
# ${bin}/pascal: ${obj}/pascal.o ${obj}/main.o | ${bin}
# 	gcc $^ -lm ${FLAGS} -o "$@"

# ${obj}/main.o: main.c | ${obj}
# 	gcc $^ -c ${FLAGS} -o "$@"
# 	@basename "$@"
# ${obj}/pascal.o: ${obj}/pascal.c | ${obj}
# 	@echo "Compiling DFA ..."
# 	gcc $^ -c ${FLAGS} -o "$@"
# 	@basename "$@"


# # Translate CSR file
# ${bin}/translate: translate.cpp | ${bin}
# 	g++ $^ -o "$@"
# ${csr}: ${bin}/translate ${in}/src-pascal.csr
# 	${bin}/translate <"${in}/src-pascal.csr" >"$@"

# # CSRPG
# ${obj}/pascal.c: ${csr} | ${obj}
# 	@echo "Building DFA ..."
# 	${csrpg} -i "${csr}" -o "${obj}/pascal.c" -h "${obj}/pascal.h" -t "${obj}/pascal-tokens.h" -g "${obj}/pascal.txt"

