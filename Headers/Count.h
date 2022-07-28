#ifndef COUNT_H_INCLUDED
#define COUNT_H_INCLUDED

#include "BOBHash32.h"
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <cstring>
#include <ctime>
template <int d, uint8_t key_len>
class CountSketch
{
private:
    int w;
    int mem_in_bytes;
    BOBHash32* hash[d];
    int* counters[d];
    BOBHash32* sign[d];
public:
    CountSketch(int bytes):mem_in_bytes(bytes)
    {
        this->w = this->mem_in_bytes / 4/ d;
        srand(time(0));
        for(int i=0; i<d; i++)
        {
            hash[i] = new BOBHash32(rand() % MAX_PRIME32);
            sign[i] = new BOBHash32(rand() % MAX_PRIME32);
            counters[i] = new int[w];
            memset(counters[i], 0, sizeof(int)*w);
        }
    }

    ~CountSketch()
    {
        for(int i=0; i<d; i++)
        {
            delete[] counters[i];
            delete hash[i];
            delete sign[i];
        }
    }

    void insert(uint8_t* key, int f =1)
    {
        for(int i=0; i<d; i++)
        {
            int idx = (hash[i]->run((const char*)key, key_len)) % w;
            int symbol = (sign[i]->run((const char*)key, key_len)) % 2;
            counters[i][idx] += symbol ? f : -f;
        }
    }

    int query(uint8_t* key)
    {
        int counter_array[d] = {0};
        for(int i=0; i<d; i++)
        {
            int idx = (hash[i]->run((const char*)key, key_len)) % w;
            int symbol = (sign[i]->run((const char*)key, key_len)) % 2;
            counter_array[i] = symbol ? counters[i][idx] : -counters[i][idx];
        }
        sort(counter_array, counter_array + d);
        int mid = d / 2, ret;
        if(mid % 2 == 0)
        {
            ret = (counter_array[mid] + counter_array[mid - 1]) / 2;
        }else
        {
            ret = counter_array[mid];
        }
        return ret;
    }

    void print_flow_table(vector<string>& flow_name, vector<pair<string, int>>& flowtable)
    {
        for(auto name: flow_name)
        {
            //cout << name << endl;
            uint8_t key[key_len] = {0};
            memcpy(key, name.c_str(), name.size());
            int count = query(key);
            pair<string, int> p = make_pair(name, count);
            flowtable.emplace_back(p);
        }
    }
};

#endif // COUNT_H_INCLUDED
