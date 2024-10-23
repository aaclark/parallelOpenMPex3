#!/bin/bash
for max in 100000000 200000000 400000000 800000000 160000000
do
  for threads in 1 2 4 8
  do
   echo "Threads= $threads max= $max "
   mpiexec -np $threads ./seives $max
  done
done
