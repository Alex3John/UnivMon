#ifndef CM_H_INCLUDED
#define CM_H_INCLUDED

#include "BOBHash32.h"
#include <vector>
#include <unordered_map>
template <int key_len, int d>
class CMSketch
{
private:
    int mem_in_bytes = 0;
    int w = 0; // the number of counters
    int* counters[d]= {NULL};
    BOBHash32* hash[d] = {NULL};
public:
    CMSketch(int mem_in_bytes)
    {
        this->mem_in_bytes = mem_in_bytes;
        initial();
    }
    void initial()
    {
        this->w = this->mem_in_bytes / 4 / d;
        for(int i=0; i<d; i++)
        {
            counters[i] = new int[w];
            memset(counters[i], 0, 4*w);
            this->hash[i] = new BOBHash32(i+750);
        }
    }
    ~CMSketch()
    {
        clear();
    }
    void clear()
    {
        for(int i=0; i<d; i++)
        {
            delete this->counters[this->w];
        }

        for(int i=0; i<d; i++)
        {
            delete this->hash[i];
        }
    }
    void insert(uint8_t* key, int f=1)
    {
        int index;
        for(int i=0; i<d; i++)
        {
            index = (hash[i]->run((const char*)key, key_len)) % w;
            counters[i][index] += f;
        }
    }
    int query(uint8_t* key)
    {
        int index;
        int ret = 1 << 30;
        for(int i=0; i<d ; i++)
        {
            index = ( hash[i]->run((const char*) key, key_len)) % w;
            ret = min(ret, counters[i][index]);
        }
        return ret;
    }
    void set_flow_info(vector<string>& flow_name, unordered_map<string, int>& flow_table)
    {
        uint8_t flow_id[key_len] = {0};
        int flow_size;
        for(auto s: flow_name)
        {
            memcpy(flow_id, s.c_str(), s.size());
            flow_size = query(flow_id);
            flow_table[s] = flow_size;
        }
    }
};

#endif // CM_H_INCLUDED
