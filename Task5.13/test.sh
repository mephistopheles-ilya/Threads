#!/bin/bash

echo "Test1"
echo "RESULT : 0.00 0.00 0.00 25.00 25.00 Changed elements :  5"
for ((i=1;i<=12;++i)); do ./a.out $i 5 tests/test_1; done | grep RESULT

echo "Test2"
echo "RESULT : 11.00 11.00 11.00 100.00 0.00 0.00 0.00 -10.00 30.00 1.00 Changed elements :  6"
for ((i=1;i<=12;++i)); do ./a.out $i 10 tests/test_2; done | grep RESULT

echo "Test3"
echo "RESULT : 3.00 7.00 5.00 5.00 5.00 Changed elements :  3"
for ((i=1;i<=12;++i)); do ./a.out $i 5 tests/test_3; done | grep RESULT

echo "Test4"
echo "RESULT : 9.00 9.00 9.00 9.00 9.00 9.00 9.00 9.00 Changed elements :  8"
for ((i=1;i<=12;++i)); do ./a.out $i 8 tests/test_4; done | grep RESULT

echo "Test5"
for((j=1;j<=10;++j))
do
    for ((i=1;i<=12;++i)); do ./a.out $i $j tests/test_5; done | grep RESULT
done

echo "Test6"
echo "RESULT : 2.00 2.00 2.00 2.00 5.00 6.00 5.00 9.00 9.00 9.00 Changed elements :  6"
for ((i=1;i<=12;++i)); do ./a.out $i 10 tests/test_6; done | grep RESULT

echo "Test7"
echo "RESULT : 100.00 2.00 5.00 2.50 2.50 2.50 2.50 9.00 0.00 77.00 Changed elements :  4"
for ((i=1;i<=12;++i)); do ./a.out $i 10 tests/test_7; done | grep RESULT

echo "Test8"
echo "RESULT : 3.00 3.00 3.00 3.00 3.00 14.00 14.00 14.00 1.00 1.00 Changed elements : 10"
for ((i=1;i<=12;++i)); do ./a.out $i 10 tests/test_8; done | grep RESULT



