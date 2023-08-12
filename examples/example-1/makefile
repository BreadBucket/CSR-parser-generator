root  = ./../..
bin   = ${root}/bin
obj   = ${root}/obj/examples

csrpg = ${root}/bin/csrpg

name  = example-1
csr   = ${name}.csr




.PHONY: all
all: ${bin}/${name}

# Execute example
.PHONY: run
run: ${bin}/${name}
	${bin}/${name}

.PHONY: clean
clean:
	rm -rf "${obj}"
	rm -f  "${bin}/${name}"



# Make directories
${bin} ${obj}:
	mkdir -p "$@"
	
# Make executable
${bin}/${name}: ${bin} main.c ${obj}/${name}.c ${obj}/${name}.h
	gcc $(filter %.c , $^) -g -o "$@"
	@echo "$@"

# Make source code
${obj}/${name}.c ${obj}/${name}.h: ${obj} ${csr}
	${csrpg} -i "${name}.csr" -o "$(patsubst %.h,%.c,$@)" -h "$(patsubst %.c,%.h,$@)" -g "$(patsubst %.c,%,$@).txt"
	@echo "$(patsubst %.h,%.c,$@)"
	@echo "$(patsubst %.c,%.h,$@)"
	@echo "$(patsubst %.c,%,$@).txt"