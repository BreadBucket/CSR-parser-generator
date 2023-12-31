#!/bin/bash

# Convert regular text file into a C const string variable.


function help(){
	echo "$0 <inputFile> <outputFile>"
}


test -f "$1" -a "$2" != "" || {
	echo "Missing input file.";
	help
	exit 1;
}


name="$(basename "$1" | tr ". " "__")"


printf "" >"$2"
printf "#define DATA_${name}\n" >>"$2"
printf "extern const char ${name}[] =" >>"$2"

s="$(cat "$1" |
	sed -e "s/[\\]/\\\\\\\\/g"	|		# Escape backslash
	sed -e 's/"/\\"/g'			|		# Escape quotes
	sed -e 's/^/"/g'			|		# Add quotes
	sed -e 's/$/\\n"/g'					# Add quotes and \n to end of line
)"


if [ ${#s} -eq 0 ]; then
	printf ' "";\n' >>"$2"
else
	printf '\n' >>"$2"
	cat >>"$2" <<<"${s%\\n\"}\""		# Remove last newline
	echo ";" >>"$2"
fi
