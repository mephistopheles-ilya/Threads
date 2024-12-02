#!/bin/bash

flag=0

for ((i=0;i<100;++i))
do
    for((n=1;n<20;++n))
    do
        ./gen $n 0 1 init.txt res.txt
        for ((j=1;j<=$n;++j))
        do
            ./a.out $j $n init.txt > my_res.txt
            echo "./a.out " $j $n " init.txt"
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
    if [ $flag -ne 0 ]; then
        break;
    fi

done

if [ $flag -ne 1 ]; then
    echo "Test Passed"
fi
        

