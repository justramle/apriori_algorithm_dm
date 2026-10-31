# Apriori Algorithm:

To discover frequent-Itemsets in transactional databases

## Description

Given a file of transactions and a minimum support threshold (as a percentage), the program will find all itemsets that appear in at least that percentage of transactions.

## Usage

```bash
g++ -std=c++17 -o myapriori apriori.cpp
./myapriori input.txt 10 && cp output.txt output_10.txt
./myapriori input.txt 20 && cp output.txt output_20.txt
./myapriori input.txt 30 && cp output.txt output_30.txt
./myapriori input.txt 50 && cp output.txt output_50.txt
```

## Generator:
https://www.philippe-fournier-viger.com/tools/Apriori_algorithm_demo.php