#!/bin/bash

flag=0

for ((i=0;i<10;++i))
do
    for((n1=1;n1<11;++n1))
    do
        for((n2=1;n2<11;++n2))
        do
            ./gen $n1 $n2 -2 2 init.txt res.txt
            for ((j=1;j<=(($n1 + 3));++j))
            do
                ./a.out $j $n1 $n2 init.txt > my_res.txt
                echo "./a.out " $j $n1 $n2 " init.txt"
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
    if [ $flag -ne 0 ]; then
        break;
    fi
done

if [ $flag -ne 1 ]; then
    echo "Test Passed"
fi
        

