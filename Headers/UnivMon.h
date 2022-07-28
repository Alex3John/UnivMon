#ifndef UNIVMON_H_INCLUDED
#define UNIVMON_H_INCLUDED

#include "BOBHash32.h"
#include "CountHeap.h"
#include <ctime>
#include <cstdlib>
#include <vector>
#include <unordered_map>
template <uint8_t key_len, uint8_t level, uint64_t mem_in_bytes>
class UnivMon
{
private:
    static constexpr uint16_t k =1000;
    typedef CountHeap<key_len, k, 5> HeavyHitterDetector;
    HeavyHitterDetector* sketches[level];
    BOBHash32* hash[level];
    int element_num = 0;
public:
    UnivMon()
    {
        int mem_for_sk = int(mem_in_bytes) - level*(key_len+4)*k;
        int mem = int(mem_for_sk / level);
        for(int i=0; i<level; i++)
        {
            sketches[i] = new HeavyHitterDetector(mem);
            hash[i] = new BOBHash32(uint32_t(rand()% MAX_PRIME32));
        }
    }
    ~UnivMon()
    {
        for(int i=0; i<level; i++)
        {
            delete sketches[i];
            delete hash[i];
        }
    }
    void insert(uint8_t* key){
        ++element_num;
        int hash_val;
        sketches[0]->insert(key);
        for(int i=1; i<level; i++)
        {
            hash_val = hash[i]->run((const char*)key, key_len) % 2;
            if(hash_val)
            {

                sketches[i]->insert(key);
            }else
            {
                break;
            }
        }
    }
    double g_sum(double (*g)(double))
    {
        int hash_val, coe;
        vector<pair<string, int>> result(k);
        double Y[level] = {0};

        for(int i = level -1; i>=0; i--)
        {
            sketches[i]->get_top_k_with_frequency(k, result);
            Y[i] = (i == level - 1) ? 0 : 2*Y[i+1];
            for(auto kv: result)
            {
                if(kv.second == 0)
                {
                    continue;
                }
                hash_val = (i == level - 1)? 1 : hash[i+1]->run(kv.first.c_str(), key_len) % 2;
                coe = (i == level - 1) ? 1 : (1 - 2*hash_val);
                Y[i] += coe*g(double(kv.second));
            }
        }
        return Y[0];
    }

    double get_cardinality()
    {
        return g_sum([](double x){return 1.0;});
    }

    double get_entropy()
    {
        double sum = g_sum([](double x){return x==0? 0 : x* std::log2(x);});
        return std::log2(element_num) - sum / element_num;
    }

    void get_heavy_hitters(uint32_t threshold, std::vector<pair<uint32_t, int> >& ret)
    {
        unordered_map<std::string, uint32_t> results;
        vector<std::pair<std::string, int>> vec_top_k(k);
        for (int i = level - 1; i >= 0; --i) {
            sketches[i]->get_top_k_with_frequency(k, vec_top_k);
            for (auto kv: vec_top_k) {
                if (results.find(kv.first) == results.end()) {
                    results[kv.first] = kv.second;
                }
            }
        }

        ret.clear();
        for (auto & kv: results) {
            if (kv.second >= threshold) {
//                results.erase(kv.first);
                ret.emplace_back(make_pair(*(uint32_t *)(kv.first.c_str()), kv.second));
            }
        }
    }
};

#endif // UNIVMON_H_INCLUDED
