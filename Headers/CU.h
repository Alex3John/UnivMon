#ifndef CU_H_INCLUDED
#define CU_H_INCLUDED

#include "BOBHash32.h"
#include <vector>
#include <unordered_map>
#include <cstring>
template <int d, uint8_t key_len>
class CUSketch
{
private:
    int w;
    int mem_in_bytes;
    int* counters[d];
    BOBHash32* hash[d];
public:
    CUSketch(int bytes):mem_in_bytes(bytes)
    {
        this->w = this->mem_in_bytes / 4 / d;
        for(int i=0; i<d; i++)
        {
            counters[i] = new int[w];
            hash[i] = new BOBHash32(i+750);
        }
    }

    ~CUSketch()
    {
        for(int i=0; i<d; i++)
        {
            delete counters[i];
            delete hash[i];
        }
    }

    void insert(uint8_t* key, uint32_t f =1)
    {
        int tmin = 1<<30;
        uint32_t index[d] = {0};
        for(int i=0; i<d; i++)
        {
            index[i] = hash[i]->run((const char*)key, key_len) % w;
            tmin = min(tmin, counters[i][index[i]]);
        }
        for(int i=0; i<d; i++)
        {
            if(counters[i][index[i]] == tmin)
            {
                counters[i][index[i]] += f;
            }
        }
    }

    int query(uint8_t* key)
    {
        int tmin = 1 << 30;
        for(int i=0; i<d; i++)
        {
            int idx = hash[i]->run((const char*) key, key_len) % w;
            tmin = min(counters[i][idx], tmin);
        }
        return tmin;
    }

    void get_flow_info(vector<string>& flow_name, unordered_map<string, int>& datamap)
    {
        uint8_t key[key_len] = {0};
        for(auto s: flow_name)
        {
            memcpy(key, s.c_str(), s.size());
            int ret = query(key);
            datamap[s] = ret;
        }
    }
};

#endif // CU_H_INCLUDED
