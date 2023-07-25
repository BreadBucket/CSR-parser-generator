#!/bin/bash

# Convert all data/* files into C string constants and then compile them.
# Generate header file containing definitions of all compiled constants.


datacpp="obj/data.cpp"
datahpp="src/data.hpp"
objFile="obj/data.o"
files=()


mkdir -p "$(dirname "$objFile")"


# Convert files
for f in $(find "data/" -type f); do
	p="obj/$(basename "${f%.*}").inc"
	files+=("$p")
	
	if [ "$f" -nt "$p" ]; then
		./datastring.sh "$f" "$p"
		echo "Baked $p"
	fi
	
done



# Build data object file
printf "// ======================== [ GENERATED FILE ] ======================== //\n" >"$datacpp"
printf "\n\n" >>"$datacpp"
printf "namespace data {\n" >>"$datacpp"
for f in ${files[@]}; do
	name="$(basename "$f")"
	printf '	#include "%s"\n' "$name" >>"$datacpp"
done;
printf "}\n" >>"$datacpp"

g++ "$datacpp" -O2 -c -o "$objFile"
# rm "$datacpp"
echo "Baked $objFile"



# Create header file
printf "// ======================== [ GENERATED FILE ] ======================== //\n" >"$datahpp"
printf "\n\n" >>"$datahpp"
printf "namespace data {\n" >>"$datahpp"
for f in ${files[@]}; do
	name="$(basename "$f")"
	printf "\textern const char %s[];\n" "${name%.*}" >>"$datahpp"
done;
printf "}\n" >>"$datahpp"

echo "Baked $datahpp"