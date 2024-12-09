#! /bin/bash

flag=0

for((n=1;n<=5678;n+=123))
do
    ./check $n > res.txt
    for((p=1;p<=10;p++))
    do
        echo $p $n
        ./a.out $p $n > my_res.txt
        cat res.txt
        cat my_res.txt
        diff -w res.txt my_res.txt
        if [ $? -ne 0 ]; then
            echo "Test failed"
            flag=1
            break
        fi 
    done
    if [ $flag -ne 0 ]; then
        break;
    fi
done

if [ $flag -ne 1 ]; then
    echo "Test Passed"
fi

