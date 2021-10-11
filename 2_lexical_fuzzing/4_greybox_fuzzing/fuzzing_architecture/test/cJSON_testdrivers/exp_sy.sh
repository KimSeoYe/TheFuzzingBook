#!/bin/sh

make testdriver_SY

start_time=$(date +%s)
for i in {1..5}
do  
   ./testdriver_SY -t 5000
   rm ./testprogram_seeds/seed_SY/mutated_*
done
end_time=$(date +%s)

echo ""
echo "======================================================="
echo "TOTAL TIME"
echo "======================================================="
echo "RANDOM MUTATION: $(($end_time-$start_time))sec"
echo ""