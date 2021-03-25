#!/bin/bash
for ((i=10;i>0;i--)); 
do 
   ./density $i method1/$i\_out.txt
   echo $i
done