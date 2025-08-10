#!/bin/bash

# Define the ranges for the loops
Wstart=200
Wend=2000
Hstart=200
Hend=1000
INCREMENT=100

mintime=1000
minW=0
minH=0

# Double for loop to iterate over the ranges
for W in $(seq $Wstart $INCREMENT $Wend); do
    for H in $(seq $Hstart $INCREMENT $Hend); do
        echo $W $H
        start_time=$(date +%s.%N)
        # ./Lab1 './testcase/case1' './result/output1.csv' $W $H
        # ./Lab1 './testcase/case2' './result/output2.csv' $W $H
        # ./Lab1 './testcase/case3' './result/output3.csv' $W $H
        # ./Lab1 './testcase/caseA' './result/outputA.csv' $W $H
        # ./Lab1 './testcase/case4' './result/output4.csv' $W $H
        # ./Lab1 './testcase/case5' './result/output5.csv' $W $H
        # ./Lab1 './testcase/case6' './result/output6.csv' $W $H
        ./Lab1 './testcase/caseB' './result/outputB.csv' $W $H
        end_time=$(date +%s.%N)
        execution_time=$(echo "$end_time - $start_time" | bc)
        echo $execution_time

        if [ "$(echo "$execution_time < $mintime" | bc)" -eq 1  ]; then
            mintime=$execution_time
            minW=$W
            minH=$H
        fi
    done
done

echo "Minimum execution time: $mintime, W: $minW, H: $minH"
