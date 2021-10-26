#!/bin/bash

make testdriver_JI

trial=60000

mut_start=$(date +%s)
for i in {1..5}
do  
   ./testdriver_JI -m -t $trial
   rm ./testprogram_seeds/seed_JI/mutated_*
done
mut_end=$(date +%s)

grey_updated_seeds=(0 0 0 0 0)

grey_start=$(date +%s)
for i in {1..5}
do  
   ./testdriver_JI -g -t $trial
   grey_updated_seeds[${i}]=$(ls -l ./testprogram_seeds/seed_JI/mutated_* | wc -l)
   rm ./testprogram_seeds/seed_JI/mutated_*
done
grey_end=$(date +%s)

echo ""
echo "======================================================="
echo "RANDOM MUTATION"
echo "-------------------------------------------------------"
echo "TIME: $(($mut_end-$mut_start))sec"
echo "\n======================================================="
echo "GREYBOX"
echo "-------------------------------------------------------"
echo "TIME: $(($grey_end-$grey_start))sec"
printf "%s" "UPDATED SEEDS: "
for i in {1..5}
do
   printf "%d " "${grey_updated_seeds[i]}"
done
echo "\n=======================================================\n"