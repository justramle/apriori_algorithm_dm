#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <cstdlib>

using namespace std;

// Read the input file and store it as a vector of transactions
vector<set<string>> readData(const string& filename) {
    vector<set<string>> transactions;
    ifstream input(filename);
    string line;

    while (getline(input, line)) {
        istringstream iss(line);
        string item;
        set<string> transaction;
        while (iss >> item) {
            transaction.insert(item);
        }
        transactions.push_back(transaction);
    }
    return transactions;
}

// Count how many transactions contain every item in the itemset
int supportCount(const set<string>& itemset, const vector<set<string>>& transactions) {
    int count = 0;
    for (int i = 0; i < transactions.size(); i++) {
        bool hasAllItems = true;
        for (set<string>::iterator it = itemset.begin(); it != itemset.end(); it++) {
            if (transactions[i].find(*it) == transactions[i].end()) {
                hasAllItems = false;
                break;
            }
        }
        if (hasAllItems) {
            count++;
        }
    }
    return count;
}

// Check whether an itemset is already in a list of itemsets
bool contains(const vector<set<string>>& list, const set<string>& target) {
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == target) {
            return true;
        }
    }
    return false;
}

// Write frequent itemsets to the output file
void writeFrequentItemsets(const string& filename, const vector<set<string>>& frequentItemsets,
                            const map<set<string>, int>& supportCounts, int minSupport, int totalTransactions) {
    ofstream output(filename);

    int minPercent = (minSupport * 100) / totalTransactions;
    output << "Minimal Support: " << minPercent << "%" << endl;

    for (int i = 0; i < frequentItemsets.size(); i++) {
        const set<string>& itemset = frequentItemsets[i];

        output << "Itemset: {";
        for (set<string>::iterator it = itemset.begin(); it != itemset.end(); it++) {
            output << *it << " ";
        }
        output << "}\t| Support Count : " << supportCounts.at(itemset) << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <filename> <minSupportPercent>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    double minSupportPercent = atof(argv[2]); // e.g. "15" means 15%
    string outputFile = "output.txt";

    vector<set<string>> transactions = readData(inputFile);

    // round the minimum support UP to the nearest whole transaction count
    double rawMinSupport = transactions.size() * minSupportPercent / 100.0;
    int minSupport = (int)rawMinSupport;
    if (rawMinSupport > minSupport) {
        minSupport++;
    }

    // Find frequent 1-itemsets (L1) 
    map<string, int> itemSupportCount;
    for (int i = 0; i < transactions.size(); i++) {
        for (set<string>::iterator it = transactions[i].begin(); it != transactions[i].end(); it++) {
            itemSupportCount[*it]++;
        }
    }

    vector<set<string>> L1;
    for (map<string, int>::iterator it = itemSupportCount.begin(); it != itemSupportCount.end(); it++) {
        if (it->second >= minSupport) {
            set<string> single;
            single.insert(it->first);
            L1.push_back(single);
        }
    }

    vector<set<string>> frequentItemsets;
    map<set<string>, int> supportCounts;
    for (int i = 0; i < L1.size(); i++) {
        frequentItemsets.push_back(L1[i]);
        supportCounts[L1[i]] = supportCount(L1[i], transactions);
    }

    // generate Ck, prune, count real support, keep frequent ones 
    int k = 2;
    while (!L1.empty()) {
        // Step 2: self-join L1 (the previous Lk-1) to generate candidate Ck
        vector<set<string>> Ck;
        for (int i = 0; i < L1.size(); i++) {
            for (int j = i + 1; j < L1.size(); j++) {
                set<string> joined = L1[i];
                for (set<string>::iterator it = L1[j].begin(); it != L1[j].end(); it++) {
                    joined.insert(*it);
                }
                if ((int)joined.size() == k && !contains(Ck, joined)) {
                    Ck.push_back(joined);
                }
            }
        }

        // Step 3: prune Ck by checking that every (k-1)-subset is in L1
        vector<set<string>> prunedCk;
        for (int i = 0; i < Ck.size(); i++) {
            set<string> candidate = Ck[i];
            bool isValid = true;
            for (set<string>::iterator it = candidate.begin(); it != candidate.end(); it++) {
                set<string> subset = candidate;
                subset.erase(*it);
                if (!contains(L1, subset)) {
                    isValid = false;
                    break;
                }
            }
            if (isValid) {
                prunedCk.push_back(candidate);
            }
        }

        // count real support and keep only itemsets meeting minSupport
        vector<set<string>> Lk;
        for (int i = 0; i < prunedCk.size(); i++) {
            int support = supportCount(prunedCk[i], transactions);
            if (support >= minSupport) {
                Lk.push_back(prunedCk[i]);
                frequentItemsets.push_back(prunedCk[i]);
                supportCounts[prunedCk[i]] = support;
            }
        }

        L1 = Lk;
        k++;
    }

    writeFrequentItemsets(outputFile, frequentItemsets, supportCounts, minSupport, (int)transactions.size());

    cout << "Frequent itemsets written to " << outputFile << endl;
    return 0;
}
