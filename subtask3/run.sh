#!/bin/bash
for ((i=1;i<=5;i++)); 
do 
	for((j=1;j<=5;j++));
	do
       echo "method 3 beta" $i $j
	   time ./COP290 $i $j 5 > out_files/method3_beta\_$i\_$j\_out.csv
	done
done
