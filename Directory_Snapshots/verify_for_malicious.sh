#!/bin/bash
if [ $# -ne 1 ] 
then
    echo "Usage: ./script1.sh inputFile" 
    exit 1
fi
input="$1"
periculos=0
linii=$(wc -l "$1" | cut -f 1 -d ' ')
cuvinte=$(wc -w "$1" | cut -f 1 -d ' ')
caractere=$(wc -c "$1" | cut -f 1 -d ' ')
if [ $linii -lt 3 ] && [ $cuvinte -gt 1000 ] && [ $caractere -gt 2000 ]
then
    periculos=0
    while read -r linie 
    do
        if grep -Pq '(corrupted|risk|dangerous|attack|malware|[\x80-\xFF])' <<< "$linie"
        then
            periculos=1
            break
        fi
    done < "$input"
fi
if [ $periculos -ne 1 ]
    then
        echo "SAFE"
    else
        echo $(realpath "$input")
fi
