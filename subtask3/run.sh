#!/bin/bash
for ((i=10;i>0;i--)); 
do 
   ./COP290 $i 1 > out_files/method1\_$i\_out.txt
   echo "method 1" $i
done

for ((i=1;i<=5;i++)); 
do 
	for((j=1;j<=5;j++));
	do
	   ./COP290 $i $j 2 > out_files/method2\_$i\_$j\_out.txt
	   echo "method 2" $i $j
	done
done

for ((i=1;i<=5;i++)); 
do 
	for((j=1;j<=5;j++));
	do
	   ./COP290 $i $j 3 > out_files/method3\_$i\_$j\_out.txt
	   echo "method 3" $i $j
	done
done

for ((i=0;i<10;i++)); 
do 
   ./COP290 $i 4 > out_files/method4\_$i\_out.txt
   echo "method 4" $i
done