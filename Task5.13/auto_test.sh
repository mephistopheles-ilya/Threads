#!/bin/bash

flag=0

for ((i=0;i<1;++i))
do
    ./gen 20 -2 2 init.txt res.txt
    for ((j=1;j<=25;++j))
    do
        ./a.out $j 20 init.txt > my_res.txt
        echo "./a.out " $j "20 init.txt"
        cat res.txt
        cat my_res.txt
        diff -w res.txt my_res.txt
        if [ $? -ne 0 ]; then
            echo "Test Faild"
            flag=1
            cat init.txt
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
        

