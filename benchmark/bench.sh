#!/bin/bash


make all

in="input/"

out_1="output/tree"
out_2="output/notree"

prog_1="./obj/program"
prog_2="./obj/program-notree"




N=100
if [ "${1:-0}" -gt "0" ]; then
	N=$1
fi


function avgFields(){
	n=0
	sum=0
	
	for t in $(grep "$2" "$1" | grep -oP "\\d.*?(?=ms)") ; do
		sum=$(awk "BEGIN{ printf \"%f\" , $sum + $t }")
		((n += 1))
	done
	
	if [ "$n" -eq "0" ]; then
		echo "0"
	else
		awk "BEGIN{ printf \"%f\" , $sum / $n }"
	fi
}




function run(){
	local name="$(basename ${1%%.*})"
	local txt="${out}/${name}.txt"
	
	mkdir -p "$out"
	rm -f "$txt"
	
	${prog} -stats <"${in}/$1" >>"$txt"
	
	# Bench lexer
	for ((i = 0 ; i < $2 ; i+=1)); do
		${prog} -lexer-parser <"${in}/$1" >>"$txt"
	done
	
	# Bench both
	for ((i = 0 ; i < $2 ; i+=1)); do
		${prog} -analysis <"${in}/$1" >>"$txt"
	done
	
	t_lexer=$(avgFields  "$txt" "LEXER")
	t_parser=$(avgFields "$txt" "PARSER")
	t_analysis=$(avgFields  "$txt" "ANALYSIS")
	t_clean=$(avgFields  "$txt" "DELETE")
	
	printf "${name}[$N]:\n"
	printf "  result:     %s\n"      "$(grep "RESULT" "${txt}" | head -n 1 | grep -oP ":.*" | grep -oP "\\w.*")"
	printf "  tokens:     %s\n"      "$(grep "TOKENS" "${txt}" | head -n 1 | grep -oP "\\d+")"
	printf "  symbols:    %s\n"      "$(grep "SYMBOLS" "${txt}" | head -n 1 | grep -oP "\\d+")"
	printf "  reductions: %s\n"      "$(grep "REDUCTIONS" "${txt}" | head -n 1 | grep -oP "\\d+")"
	printf "  steps:      %s\n"      "$(grep "STEPS" "${txt}" | head -n 1 | grep -oP "\\d+")"
	printf "  LEXER:      %.3fms\n"  "$t_lexer"
	printf "  PARSER:     %.3fms\n"  "$t_parser"
	printf "  ANALYSIS:   %.3fms\n"  "$t_analysis"
	printf "  CLEANUP:    %.3fms\n"  "$t_clean"
}




out="$out_1"
prog="$prog_1"
f="${out}/time.txt"
rm -f "$f"

run "tangle.p" $N | tee -a "$f"
printf "\n"    $N | tee -a "$f"
run "weave.p"  $N | tee -a "$f"
printf "\n"    $N | tee -a "$f"
run "tex.p"    $N | tee -a "$f"
printf "\n"    $N | tee -a "$f"
run "mf.p"     $N | tee -a "$f"
printf "\n"    $N | tee -a "$f"


out="$out_2"
prog="$prog_2"
f="${out}/time.txt"
rm -f "$f"

run "tangle.p" $N | tee -a "$f"
printf "\n"    $N | tee -a "$f"
run "weave.p"  $N | tee -a "$f"
printf "\n"    $N | tee -a "$f"
run "tex.p"    $N | tee -a "$f"
printf "\n"    $N | tee -a "$f"
run "mf.p"     $N | tee -a "$f"
printf "\n"    $N | tee -a "$f"


