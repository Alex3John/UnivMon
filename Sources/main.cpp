#include <iostream>
#include "UnivMon.h"
#include <fstream>
#include <cstring>
using namespace std;
#include <stdio.h>
#define key_len 24
#define mem_in_bytes (1024*600)
#define level 14
UnivMon<key_len, level, mem_in_bytes> * um = new UnivMon<key_len, level, mem_in_bytes>();
vector<pair<uint32_t, int>> heavyhitters;
void ShowHH()
{
    string key;
    int count = 0;
    for(auto kv: heavyhitters)
    {
        printf(" <%.8x, %d> ", kv.first, kv.second);
        if(++count == 5)
        {
            cout << endl;
        }
    }
}

int main()
{
    ifstream in;
    in.open("F:\\001.txt"); // 文件每行为一个IP地址  CAIDA2016
    string srcIP;
    while(in)
    {
        uint8_t key[key_len] = {0};
        in >> srcIP;
        memcpy(key, srcIP.c_str(), srcIP.size());
        um->insert(key);
    }
    cout << "**********Cardinality**********" << endl;
    int cardinality = um->get_cardinality();
    cout << cardinality << endl;
    cout << "**********Entropy**********" << endl;
    double entropy = um->get_entropy();
    cout << entropy << endl;
    um->get_heavy_hitters(10, heavyhitters);
    ShowHH();
    in.close();
    return 0;
}
