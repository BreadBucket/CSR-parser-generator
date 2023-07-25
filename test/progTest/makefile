project = ../../

src_only = $(shell grep -oP "\\S*\\.c(pp)?" <<<"$^")



.PHONY: all
all: generatedProgram


.PHONY: generatedProgram
generatedProgram:
	gcc "main.c" -g -o "${project}/obj/generatedProgram"


.PHONY: run
run: generatedProgram
	./${project}/obj/generatedProgram