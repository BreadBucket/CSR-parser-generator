#!/bin/bash

in="input/"
out="output/"

prog="$(make getProg)"
make ${prog}


N=100
if [ "$1" -gt 0 ]; then
	N=$1
fi


function avgFields(){
	time=( $(grep "$2" "$1" | grep -oP "\\d.*?(?=ms)") )
	
	sum=0
	for t in ${time[*]}; do
		sum=$(awk "BEGIN{ printf \"%f\" , $sum + $t }")
	done
	
	awk "BEGIN{ printf \"%f\" , $sum / ${#time[*]} }"
}


function run(){
	local name="$(basename ${1%%.*})"
	local txt="${out}/${name}.txt"
	
	rm -f "$txt"
	
	# echo "${name}:"
	for ((i = 0 ; i < $2 ; i+=1)); do
		# s=$(cat "${in}/$1" | ./lexer | ${prog})
		# printf "  [%d/%d]  " "$i" "$N"
		# grep "TOTAL" <<<"$s" | grep -oP "\\d+(\.\\d+)?ms"
		# cat <<<"$s" >>"$txt"
		
		cat "${in}/$1" | ./lexer | ${prog} ${arg} >>"$txt"
	done
	
	
	printf "${name}[$N]:\n"
	printf "  RESULT: %s\n"      "$(grep "Result" "${txt}" | head -n 1 | grep -oP ":.*" | grep -oP "\\w.*")"
	printf "  TOKENS: %s\n"      "$(grep "Tokens" "${txt}" | head -n 1 | grep -oP "\\d+")"
	printf "  INPUT:  %.3fms\n"  "$(avgFields "$txt" "INPUT")"
	printf "  DFA:    %.3fms\n"  "$(avgFields "$txt" "DFA")"
	printf "  DELETE: %.3fms\n"  "$(avgFields "$txt" "DELETE")"
	printf "  TOTAL:  %.3fms\n"  "$(avgFields "$txt" "TOTAL")"
}




arg="-f0"
rm -f "${out}/time.txt"
run "tangle.p" $N | tee -a "${out}/time.txt"
printf "\n"    $N | tee -a "${out}/time.txt"
run "weave.p"  $N | tee -a "${out}/time.txt"
printf "\n"    $N | tee -a "${out}/time.txt"
run "tex.p"    $N | tee -a "${out}/time.txt"
printf "\n"    $N | tee -a "${out}/time.txt"
run "mf.p"     $N | tee -a "${out}/time.txt"
printf "\n"    $N | tee -a "${out}/time.txt"

# Remove INPUT field
grep -v "INPUT" "${out}/time.txt" >"${out}/_time.txt"
rm -rf "${out}/time.txt"
mv "${out}/_time.txt" "${out}/time.txt"


arg="-f1"
rm -f "${out}/time_buffered.txt"
run "tangle.p" $N | tee -a "${out}/time_buffered.txt"
printf "\n"    $N | tee -a "${out}/time_buffered.txt"
run "weave.p"  $N | tee -a "${out}/time_buffered.txt"
printf "\n"    $N | tee -a "${out}/time_buffered.txt"
run "tex.p"    $N | tee -a "${out}/time_buffered.txt"
printf "\n"    $N | tee -a "${out}/time_buffered.txt"
run "mf.p"     $N | tee -a "${out}/time_buffered.txt"
printf "\n"    $N | tee -a "${out}/time_buffered.txt"


