SHELL := /bin/bash

src = src/
inc = -I src/includes/ -I src/includes/util/ -I src/includes/Parser/ -I src/includes/DFA/ -I src/includes/Generator/
compilerPath = obj/compiler.mk

gcc_options = -std=c++2a -g

src_only = $(shell grep -oP "\\S*\\.c(pp)?" <<<"$^")
src_finder = find ${src} -type f -iname "*.c" -or -iname "*.cpp"




.PHONY: all
all: ${compilerPath} obj/program




.PHONY: run
run: all
	@./obj/program -i "test/test.csr" --graph="obj/graph.txt" --tab=1

.PHONY: runGen
runGen:
	@make -C test/progTest run




.PHONY: clean
clean:
	rm -rf "obj/"
	rm -f ${compilerPath}
	@echo "Project clean."




# Bake data files into data.o, included in compiler.mk
.PHONY: bake
bake:
	rm -f obj/data.o
	rm -f obj/*.inc
	make obj/data.o

obj/data.o: $(shell find "data/" -type f)
	./bake.sh
	@touch obj/data.o


# Build makefile target dependencies on all source files
.PHONY: compiler
compiler:
	rm -f "${compilerPath}"
	@make "${compilerPath}"
	@echo "Done."

${compilerPath}:
	mkdir -p "$(shell dirname "${compilerPath}")"
	
	@echo "Generating source list: ${compilerPath}"
	@bash -c '																	\
		printf "# Auto-generated file. Do not touch!\n\n" >"${compilerPath}" ;	\
																				\
		while read line; do														\
			test -n "$${line}" && {												\
				line=obj/$${line} ;												\
				echo "$${line}" >>"${compilerPath}" ;							\
				echo -e "\t@basename \"\$$@\"" >>"${compilerPath}" ;			\
				echo -e "\t@g++ \$${src_only} \$${inc} -c \$${gcc_options} -o \$$@" >>"${compilerPath}" ;	\
				obj+=( $$(echo "$$line" | grep -oP "^\S+\.o") ) ;			\
			};																\
		done <<<"$$(g++ ${inc} -MM $$(${src_finder}))" ;					\
																			\
		echo "" >>"${compilerPath}" ;										\
		echo "obj/program: obj/data.o $${obj[*]}" >>"${compilerPath}" ;		\
		echo -e "\t@basename \"\$$@\"" >>"${compilerPath}" ;				\
		echo -e "\t@g++ \$$^ \$${gcc_options} -o \$$@" >>"${compilerPath}"	\
	'

ifneq (clean,$(filter clean,$(MAKECMDGOALS)))
include ${compilerPath}
endif