SHELL := /bin/bash

src			= src/
inc         = -I src/includes/
gcc_options = -std=c++2a -g

compilerPath	= "compiler.mk"
src_only		= $(shell grep -oP "\\S*\\.c(pp)?" <<<"$^")
src_finder		= find ${src} -type f -iname "*.c" -or -iname "*.cpp"




.PHONY: all
all: compiler.mk obj obj/program


.PHONY: run
run: all
	@./obj/program


obj:
	@mkdir obj


.PHONY: clean
clean:
	rm -rf "obj/"
	rm ${compilerPath}
	@echo "Project clean."



.PHONY: compiler
compiler:
	rm compiler.mk
	make compiler.mk

compiler.mk:
	@echo Generating source list: compiler.mk
	@bash -c '															\
		echo "# Auto-generated file. Do not touch!" >${compilerPath} ;	\
		echo "" >>${compilerPath} ;										\
																		\
		while read line; do												\
			test -n "$${line}" && {										\
				line=obj/$${line} ;										\
				echo "$${line}" >>${compilerPath} ;						\
				echo -e "\t@basename \"\$$@\"" >>${compilerPath} ;		\
				echo -e "\t@g++ \$${src_only} \$${inc} -c \$${gcc_options} -o \$$@" >>${compilerPath} ;	\
				obj+=( $$(echo "$$line" | grep -oP "^\S+\.o") ) ;			\
			};																\
		done <<<"$$(g++ ${inc} -MM $$(${src_finder}))" ;					\
																			\
		echo "" >>${compilerPath} ;											\
		echo "obj/program: $${obj[*]}" >>${compilerPath} ;					\
		echo -e "\t@echo program" >>${compilerPath} ;						\
		echo -e "\t@g++ \$$^ \$${gcc_options} -o \$$@" >>${compilerPath}	\
	'
	
include compiler.mk