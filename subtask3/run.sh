#!/bin/bash
for ((i=1;i<=10;i++));
do
    echo "method 4" $i
    time ./COP290 $i 4 > out_files/method4\_$i\_out.csv
done
