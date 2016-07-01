#!/bin/bash

OUTPUTFILE=$1

FLAG=0
IFS='\n'
COUNT=0

declare -a lines=`cat ftraceop.txt | tr -d " \t1)" | grep -A 6 "napi_complete_done("`

#IFS='|'

#cat $OUTPUTFILE | tr -d " \t1)" | grep "__local_bh_enable();"

for (( i=0; i < ${#lines[*]}; ++ i ))
do
	#echo ${lines[$i]}
	if [ "${lines[$i]}" == "0.028us|__local_bh_e able(;" ]; then
		echo ${#lines[$i]}
	fi
done
