root  = ./../..
bin   = ${root}/bin
obj   = ${root}/obj/pascal

in = input
out = output

csrpg = ${root}/bin/csrpg
csr   = ${in}/pascal.csr

FLAGS = -O2




.PHONY: all
all: ${bin}/pascal

.PHONY: run
run: ${bin}/pascal
	cat "${in}/mf.p" | ./lexer | ${bin}/pascal

# .PHONY: translate
# translate: ${bin}/translate
# 	${bin}/translate <"old-pascal.csr" >"pascal.csr"

# .PHONY: test
# test: ${bin}/translate
# 	${bin}/translate <debug.txt

.PHONY: clean
clean:
	rm -rf "${obj}" "${bin}/translate" "${bin}/pascal"
	rm -f "${csr}"
	@echo "Project cleaned."




# Directories
${obj} ${bin}:
	mkdir -p "$@"


# Program
${bin}/pascal: ${obj}/pascal.o ${obj}/main.o | ${bin}
	gcc $^ -lm ${FLAGS} -o "$@"

${obj}/main.o: main.c | ${obj}
	@gcc $^ -c ${FLAGS} -o "$@"
	@basename "$@"
${obj}/pascal.o: ${obj}/pascal.c | ${obj}
	@gcc $^ -c ${FLAGS} -o "$@"
	@basename "$@"


# Translate CSR file
${bin}/translate: translate.cpp | ${bin}
	g++ $^ -o "$@"
${csr}: ${bin}/translate ${in}/src-pascal.csr
	${bin}/translate <"${in}/src-pascal.csr" >"$@"

# CSRPG
${obj}/pascal.c: ${csr} | ${obj}
	${csrpg} -i "${csr}" -o "${obj}/pascal.c" -h "${obj}/pascal.h" -t "${obj}/pascal-tokens.h" -g "${obj}/pascal.txt"

