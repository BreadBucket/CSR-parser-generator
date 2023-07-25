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


name="$(basename "${1%.*}")"


printf "" >"$2"
printf "#define DATA_${name}\n" >>"$2"
printf "extern const char ${name}[] =\n" >>"$2"

cat "$1" |
	sed -e 's/"/\\"/g'	|			# Escape quotes
	sed -e 's/^/"/g'	|			# Add quotes to start of line
	sed -e 's/$/\\n"/g'	>>"$2"		# Add quotes and \n to end of line
	
printf ";\n" >>"$2"
