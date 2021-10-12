#!/bin/sh

make testdriver_WS

trial=5000

mut_start=$(date +%s)
for i in 1 2 3 4 5
do  
   ./testdriver_WS -m -t $trial
done
mut_end=$(date +%s)

grey_start=$(date +%s)
for i in 1 2 3 4 5
do  
   ./testdriver_WS -g -t $trial
   rm ./testprogram_seeds/seed_WS/mutated_*
done
grey_end=$(date +%s)

echo ""
echo "======================================================="
echo "TOTAL TIME"
echo "======================================================="
echo "RANDOM MUTATION: $(($mut_end-$mut_start))sec"
echo "GREYBOX: $(($grey_end-$grey_start))sec"
echo ""