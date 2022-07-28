#ifndef LINEARCOUNTING_H_INCLUDED
#define LINEARCOUNTING_H_INCLUDED

#include "BOBHash32.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
template <int mem_in_bytes, uint8_t key_len>
class LinearCounting
{
private:
    uint8_t bitmap[mem_in_bytes];
    BOBHash32* bithash;
public:
    LinearCounting()
    {
        bithash =  new BOBHash32(rand() % MAX_PRIME32);
        memset(bitmap, 0, mem_in_bytes);
    }
    ~LinearCounting()
    {
        delete[] bitmap;
    }
    void insert(uint8_t* key)
    {
        int pos = bithash->run((const char*)key, key_len) % (mem_in_bytes * 8);
        int pos1 = pos / 8, pos2 = pos % 8;
        bitmap[pos1] = bitmap[pos1] | (1 << pos2);
    }
    int get_cardinality()
    {
        int m = mem_in_bytes * 8;
        double z = 0.0;
        for(int i=0; i<mem_in_bytes; i++)
        {
            z += __builtin_popcount(bitmap[i]);
        }
        return m * log(m/ (m - z));
    }
};

#endif // LINEARCOUNTING_H_INCLUDED
