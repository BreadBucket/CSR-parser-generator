#!/bin/bash

# Convert all data/* files into C string constants and then compile them.
# Generate header file containing definitions of all compiled constants.

dataDir="obj/data"
objFile="obj/data.o"
datacpp="$dataDir/data.cpp"
datahpp="src/data.hpp"
files=()


mkdir -p "$dataDir"
fresh=0


# Convert files
for f in $(find "data/" -type f | sort); do
	name="$(basename "$f" | tr ". " "__")"
	p="$dataDir/${name}.inc"
	files+=("$p")
	
	if [ "$f" -nt "$p" ]; then
		fresh=1
		./datastring.sh "$f" "$p"
		echo "Baked $p"
	fi
	
done


# Nothing changed, exit
if [ "$fresh" != "1" ]; then
	exit 0
fi


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
echo "Baked $objFile"




# Create header file
printf "// ======================== [ GENERATED FILE ] ======================== //\n" >"$datahpp"
# printf "#include <string_view>\n" >>"$datahpp"

printf "\n\n" >>"$datahpp"
printf "namespace data {\n" >>"$datahpp"
for f in ${files[@]}; do
	name="$(basename "${f%.*}")"
	printf "\textern const char %s[];\n" "$name" >>"$datahpp"
done;
printf "}\n" >>"$datahpp"

# printf "\n\n" >>"$datahpp"
# printf "namespace data {\n" >>"$datahpp"
# for f in ${files[@]}; do
# 	name="$(basename "${f%.*}")"
# 	printf "\textern const std::string_view %s;\n" "$name" >>"$datahpp"
# done;
# printf "}\n\n" >>"$datahpp"


echo "Baked $datahpp"