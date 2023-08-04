SHELL := /bin/bash

src = src/
inc = -I src/includes/ -I src/includes/util/ -I src/includes/Parser/ -I src/includes/DFA/ -I src/includes/Generator/
compilerPath = obj/compiler.mk

gcc_options = -std=c++2a -g
src_finder = find ${src} -type f -iname "*.c" -or -iname "*.cpp"




.PHONY: all
all: ${compilerPath} obj/program




.PHONY: run
run: all
	@./obj/program -i "test/test.csr" --tab=1 \
		--graph="obj/graph.txt" \
		--output="obj/switch.c" \
		--header="obj/switch.h"	\
		--token="obj/token.h"

.PHONY: runGen
runGen:
	@make -C test/progTest run --no-print-directory




.PHONY: clean
clean:
	rm -rf "obj/"
	rm -f ${compilerPath}
	@echo "Project clean."




# Bake data files into data.o, included in compiler.mk
.PHONY: rebake
rebake:
	rm -f obj/data.o
	rm -f obj/*.inc
	./bake.sh
	@touch obj/data.o

.PHONY: bake
bake:
	./bake.sh
	@touch obj/data.o

obj/data.o: $(shell find "data/" -type f)
	./bake.sh
	@touch obj/data.o


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
				line=obj/$${line} ;											\
				echo "$${line}" >>"$${temp}" ;								\
				echo -e "\t@basename \"\$$@\"" >>"$${temp}" ;				\
				echo -e "\t@g++ \$$(filter %.c %.cpp %.inc, \$$^) \$${inc} -c \$${gcc_options} -o \$$@" >>"$${temp}" ;	\
				obj+=( $$(echo "$$line" | grep -oP "^\S+\.o") ) ;		\
			};															\
		done <<<"$$(g++ ${inc} -MM $$(${src_finder}))" ;				\
																		\
		echo "" >>"$${temp}" ;											\
		echo "obj/program: obj/data.o $${obj[*]}" >>"$${temp}" ;		\
		echo -e "\t@basename \"\$$@\"" >>"$${temp}" ;					\
		echo -e "\t@g++ \$$^ \$${gcc_options} -o \$$@" >>"$${temp}" ;	\
																		\
		cat "$${temp}" >"${compilerPath}" ;								\
		rm "$${temp}"													\
	'

ifeq (,$(filter clean bake obj/data.o compiler,$(MAKECMDGOALS)))
include ${compilerPath}
endif
