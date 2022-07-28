#ifndef FM_H_INCLUDED
#define FM_H_INCLUDED

#include <ctime>
#include <cstdlib>
#include "BOBHash32.h"
#include <cstring>
#include <stdio.h>
template <int mem_in_bytes, uint8_t key_len>
class FMSketch
{
private:
    uint8_t counter[mem_in_bytes];
    BOBHash32* BOB;
public:
    FMSketch()
    {
        BOB =  new BOBHash32(rand() % MAX_PRIME32);
        memset(counter, 0, sizeof(counter));
    }

    ~FMSketch()
    {
        delete[] counter;
        delete BOB;
    }

    void insert(uint8_t* key)
    {
        int idx = BOB->run((const char*)key, key_len) % (mem_in_bytes*8);
        int pos1 = idx / 8;
        int idx2 = idx % 8;
        int b=0;
        while((idx2 % 2 == 0) && idx2!=0)
        {
            idx2 = idx2 >> 1;
            b++;
        }
        //cout << "begin:" << counter[pos1] << endl;
        //printf("begin: %u, b=%d\n, 1<<b = %d\n", counter[pos1], b, 1<<b);
        counter[pos1] = counter[pos1] | (0x10000000 >> b);
        //cout << "end:" << counter[pos1] << endl;
        //printf("end: %u\n", counter[pos1]);
    }

    int get_cardinality()
    {
        int z=-1;
        int bit_pos=-1;
        for(int i=0; i<mem_in_bytes; i++)
        {

            if(__builtin_popcount(counter[i] & 0x10000000) == 0)
            {
                bit_pos = 7;
            }else if(__builtin_popcount(counter[i] & 0x01000000) == 0)
            {
                bit_pos = 6;
            }else if(__builtin_popcount(counter[i] & 0x00100000) == 0)
            {
                bit_pos = 5;
            }else if(__builtin_popcount(counter[i] & 0x00010000) == 0)
            {
                bit_pos = 4;
            }else if(__builtin_popcount(counter[i] & 0x00001000) == 0)
            {
                bit_pos = 3;
            }else if(__builtin_popcount(counter[i] & 0x00000100) == 0)
            {
                bit_pos = 2;
            }else if(__builtin_popcount(counter[i] & 0x00000010) == 0)
            {
                bit_pos = 1;
            }else if(__builtin_popcount(counter[i] & 0x01000001) == 0)
            {
                bit_pos = 0;
            }else
            {
                continue;
            }
            if(bit_pos >= 0)
            {
                z = i*8 + bit_pos;
                break;
            }
        }
        cout << z << endl;
        return (1 << z) / 0.77351;
    }
};

#endif // FM_H_INCLUDED
