#!/bin/bash

make testdriver_JI

trial=6000

mut_start=$(date +%s)
for i in {1..5}
do  
   ./testdriver_JI -m "ji_mut_"${i}".csv" -t $trial
   rm ./testprogram_seeds/seed_JI/mutated_*
done
mut_end=$(date +%s)

grey_updated_seeds=(0 0 0 0 0)

grey_start=$(date +%s)
for i in {1..5}
do  
   ./testdriver_JI -g "ji_grey_"${i}".csv" -t $trial
   grey_updated_seeds[${i}]=$(ls -l ./testprogram_seeds/seed_JI/mutated_* | wc -l)
   rm ./testprogram_seeds/seed_JI/mutated_*
done
grey_end=$(date +%s)

echo ""
echo "======================================================="
echo "RANDOM MUTATION TIME: $(($mut_end-$mut_start))sec"
echo "GREYBOX TIME: $(($grey_end-$grey_start))sec"
printf "%s" "UPDATED SEEDS: "
for i in {1..5}
do
   printf "%d " "${grey_updated_seeds[i]}"
done
echo ""
echo "======================================================="
echo ""