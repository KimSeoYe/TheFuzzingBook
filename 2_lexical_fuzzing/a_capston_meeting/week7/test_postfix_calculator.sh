#!bin/bash

input_arr=("1 1 - 2 + 3 +;" "3 2 2 + * ;" "1  2  + 3 ^ ;" "3 / 2 ;" "3 0 / ;" "3 2 / ")
result_summary=("pass" "pass" "pass" "pass" "pass" "pass")

for i in {0..5};
do
	echo "${input_arr[i]}" | ./simple_postfix_calculator > ./results/case${i}
	if [ $? != '0' ]; then
		result_summary[${i}]="fail"
	fi
done

echo "========================"
echo "RESULTS"
echo "========================"
for i in {0..5};
do
	printf "[${i}] ${result_summary[i]}\n"
done
echo "========================"
