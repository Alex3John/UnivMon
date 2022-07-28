#ifndef CUHEAP_H_INCLUDED
#define CUHEAP_H_INCLUDED

#include "BOBHash32.h"
#include <unordered_map>
#include "cuckoohashing.h"
#include <algorithm>
template <int d, uint8_t key_len, int capacity>
class CUHeap
{
private:
    typedef pair<uint8_t[key_len], int> KV;
    int mem_in_byte;
    int w;
    BOBHash32* hash[d];
    int* counters[d];
    int heap_element_num;
    KV heap[capacity];
    cuckoo::CuckooHashing<key_len, int(capacity * 2), d> ht;
public:
    void heap_adjust_down(int i)
    {
        while(i < this->heap_element_num / 2)
        {
            int l_child = 2 * i + 1;
            int r_child = 2 * i + 2;
            int larger_one = i;
            if(l_child < this->heap_element_num && heap[l_child].second < heap[larger_one].second)
            {
                larger_one = l_child;
            }
            if(r_child < this->heap_element_num && heap[r_child].second < heap[larger_one].second)
            {
                larger_one = r_child;
            }
            if(larger_one != i)
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
    void heap_adjust_on(int i)
    {
        while(i > 1)
        {
            int parent = (i - 1)/2;
            if(heap[parent].second <= heap[i].second)
            {
                break;
            }
            swap(heap[parent], heap[i]);
            swap(ht[heap[parent].first], ht[heap[i].first]);
            i = parent;
        }
    }
    CUHeap(int bytes):mem_in_byte(bytes),heap_element_num(0)
    {
        this->w = this->mem_in_byte / 4 / d;
        for(int i=0; i<d; i++)
        {
            hash[i] = new BOBHash32(i+750);
            counters[i] = new int[w];
            memset(counters[i], 0, sizeof(int)*w);
        }
        memset(heap, 0, capacity);
        for(int i=0; i<capacity; i++)
        {
            heap[i].second = 0;
        }
    }
    ~CUHeap()
    {
        for(int i=0; i<d; i++)
        {
            delete counters[i];
            delete hash[i];
        }
    }
    void insert(uint8_t* key, int f=1)
    {
        int tmin = 1 << 30;
        int index[d];
        for(int i=0; i<d; ++i)
        {
           index[i] = hash[i]->run((const char*)key, key_len) % w;
           tmin = min(tmin, counters[i][index[i]]);
        }
        for(int i=0; i<d; i++)
        {
            if(counters[i][index[i]] == tmin)
            {
                counters[i][index[i]] ++;
            }
        }
        if(ht.find(key))
        {
            // cout << "ÕÒµ½ÁË" << endl;
            heap[ht[key]].second += f;
            heap_adjust_down(ht[key]);
        }else if(this->heap_element_num < capacity)
        {

            memcpy(heap[heap_element_num].first, key, key_len);
            heap[heap_element_num].second = tmin;
            ht[key] = heap_element_num ++ ;
            heap_adjust_on(this->heap_element_num - 1);
        }else if(heap[0].second < tmin)
        {
            KV& top_element = heap[0];
            ht.erase(top_element.first);
            memcpy(top_element.first, key, key_len);
            top_element.second = tmin;
            ht[key] = 0;
            heap_adjust_down(0);
        }
    }

    int query(uint8_t* key)
    {
        int tmin = 1<<30;
        for(int i=0; i<d; i++)
        {
            int idx = hash[i]->run((const char*)key, key_len) % (this->w);
            tmin = min(tmin, counters[i][idx]);
        }
        return tmin;
    }

    void get_heavy_hitter(vector<pair<string,int>>& datavector)
    {
        //cout <<"¶ÑÔªËØ" << heap_element_num << endl;
        for(auto e: heap)
        {
            pair<string, int> p = make_pair(string((char*)e.first, key_len), e.second);
            datavector.emplace_back(p);
        }
    }
};

#endif // CUHEAP_H_INCLUDED
