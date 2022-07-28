#ifndef CMHEAP_H_INCLUDED
#define CMHEAP_H_INCLUDED

#include <unordered_map>
#include <algorithm>
#include "BOBHash32.h"
#include "cuckoo_hashing.h"

template <uint8_t key_len, int capacity, int d = 4>
struct CMHeap
{
    typedef pair<uint8_t[key_len], int> KV;
    KV heap[capacity];
    int heap_element_num;
    int mem_in_bytes;
    int w;
    int * counters[d];
    BOBHash32* hash[d];
    cuckoo::CuckooHashing<key_len, int(capacity * 2)> ht;

    void heap_adjust_down(int i)
    {
        while(i < heap_element_num / 2)
        {
            int l_child = 2 * i + 1;
            int r_child = 2 * i + 2;
            int larger_one = i;
            if(l_child < heap_element_num && heap[l_child].second < heap[larger_one].second)
            {
                larger_one = l_child;
            }
            if(r_child < heap_element_num && heap[r_child].second < heap[larger_one].second)
            {
                larger_one = r_child;
            }
            if(larger_one!=i)
            {
                swap(heap[i], heap[larger_one]);
                swap(ht[heap[i].first], ht[heap[larger_one].first]);
                heap_adjust_down(larger_one);
            }else
            {
                break;
            }
        }
    }

    void heap_adjust_up(int i)
    {
        while(i > 1)
        {
            int parent = (i - 1) / 2;
            if(heap[parent].second < heap[i].second)
            {
                break;
            }
            swap(heap[parent], heap[i]);
            swap(ht[heap[parent].first], ht[heap[i].first]);
            i = parent;
        }
    }

public:
    CMHeap(int mem_in_byte_):mem_in_bytes(mem_in_byte_), heap_element_num(0)
    {
        w = mem_in_bytes / 4 /d;
        for(int i = 0; i<d ; i++)
        {
            counters[i] = new int[w];
            memset(counters[i], 0, sizeof(int) * w);
        }
        memset(heap, 0, sizeof(heap));
        for(int i=0; i< capacity; i++)
        {
            heap[i].second = 0;
        }
        for(int i=0; i<d; i++)
        {
            hash[i] = new BOBHash32(i+750);
        }
    }

    void insert(uint8_t* key)
    {
        int tmin = 1 << 30, ans=tmin;
        for(int i=0; i<d; i++)
        {
            int index = hash[i]->run((char*)key, key_len) % w;
            counters[i][index]++;
            int val = counters[i][index];

            ans = min(val, ans);
        }
        string str_key = string((const char*)key, key_len);
        if(ht.find(key))
        {
            heap[ht[key]].second ++ ;
            heap_adjust_down(ht[key]);
        }else if(heap_element_num < capacity)
        {
            memcpy(heap[heap_element_num].first, key, key_len);
            heap[heap_element_num].second = ans;
            ht[key] = heap_element_num++;
            heap_adjust_up(heap_element_num - 1);
        }else if(ans > heap[0].second)
        {
            KV& kv = heap[0];
            ht.erase(kv.first);
            memcpy(kv.first, key, key_len);
            kv.second = ans;
            ht[key] = 0;
            heap_adjust_down(0);
        }
    }

    int query(uint8_t* key)
    {
        int tmin = 1<<30, ans = tmin;
        for(int i=0; i<d; i++)
        {
            int index = hash[i]->run((const char*)key, key_len) % w;
            int val = counters[i][index];
            ans = min(val, ans);
        }
        return ans;
    }

    void get_heavy_hitter(uint32_t threshold, vector<pair<string, uint32_t>>& ret)
    {
        ret.clear();
        for(int i=0; i<capacity; i++)
        {
            if(heap[i].second >= threshold)
            {
                ret.emplace_back(make_pair(string((char*)heap[i].first, key_len), heap[i].second));
            }
        }
    }
    ~CMHeap()
    {
        for(int i=0; i<d; ++i)
        {
            delete counters[i];
            delete hash[i];
        }
    }
};

#endif // CMHEAP_H_INCLUDED
