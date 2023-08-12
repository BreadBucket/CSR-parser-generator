SHELL := /bin/bash

src = src
obj = obj
bin = bin

inc = -I src/includes/ -I src/includes/util/ -I src/includes/Parser/ -I src/includes/DFA/ -I src/includes/Generator/
compilerPath = ${obj}/compiler.mk

gcc_options = -std=c++2a -g
src_finder = find ${src} -type f -iname "*.c" -or -iname "*.cpp"




.PHONY: all
all: ${obj} ${compilerPath} ${bin}/csrpg

# Run CSRPG
.PHONY: run
run: all
	@make -C test gen --no-print-directory

# Run test program
.PHONY: runTest
runTest:
	@make -C test run --no-print-directory

# Remove generated files
.PHONY: clean
clean:
	rm -rf "${obj}" "${bin}"
	rm -f ${compilerPath}
	@echo "Project clean."




# Create directories
${obj} ${bin}:
	mkdir "$@"




# Bake data files into data.o, included in compiler.mk
.PHONY: rebake
rebake:
	rm -f "${obj}/data.o"
	rm -rf "${obj}/data"
	./bake.sh
	@touch ${obj}/data.o

.PHONY: bake
bake:
	./bake.sh
	@touch obj/data.o

${obj}/data.o: $(shell find "data/" -type f)
	./bake.sh
	@touch ${obj}/data.o




# Build makefile target dependencies on all source files
.PHONY: compiler
compiler:
	rm -f "${compilerPath}"
	@make "${compilerPath}" --no-print-directory
	@echo "Done."

${compilerPath}:
	@mkdir -p "$(shell dirname "${compilerPath}")"
	
	@echo "Generating source list: ${compilerPath}"
	@bash -c '																\
		temp="$$(mktemp)" ;													\
		printf "# Auto-generated file. Do not touch!\n\n" >"$${temp}" ;		\
																			\
		while read line; do													\
			test -n "$${line}" && {											\
				line=${obj}/$${line} ;										\
				echo "$${line}" >>"$${temp}" ;								\
				echo -e "\t@basename \"\$$@\"" >>"$${temp}" ;				\
				echo -e "\t@g++ \$$(filter %.c %.cpp %.inc, \$$^) \$${inc} -c \$${gcc_options} -o \$$@" >>"$${temp}" ;	\
				obj+=( $$(echo "$$line" | grep -oP "^\S+\.o") ) ;						\
			};																			\
		done <<<"$$(g++ ${inc} -MM $$(${src_finder}))" ;								\
																						\
		echo "" >>"$${temp}" ;															\
		echo "${bin}/csrpg: ${bin} obj/data.o $${obj[*]}" >>"$${temp}" ;				\
		echo -e "\t@basename \"\$$@\"" >>"$${temp}" ;									\
		echo -e "\t@g++ \$$(filter %.o, \$$^) \$${gcc_options} -o \$$@" >>"$${temp}" ;	\
																						\
		cat "$${temp}" >"${compilerPath}" ;												\
		rm "$${temp}"																	\
	'

ifeq (,$(MAKECMDGOALS))
include ${compilerPath}
else ifneq (,$(filter ${obj}/%.o ${bin}/% all run ,$(MAKECMDGOALS)))
include ${compilerPath}
endif
