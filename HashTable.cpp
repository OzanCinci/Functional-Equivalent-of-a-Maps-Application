#include "HashTable.h"

#include <cassert>
#include <iostream>
#include <iomanip>

const int KeyedHashTable::PRIME_LIST[PRIME_TABLE_COUNT] =
    {
        2, 3, 5, 7, 11, 13, 17, 19,
        23, 29, 31, 37, 41, 43, 47, 53,
        59, 61, 67, 71, 73, 79, 83, 89,
        97, 101, 103, 107, 109, 113, 127, 131,
        137, 139, 149, 151, 157, 163, 167, 173,
        179, 181, 191, 193, 197, 199, 211, 223,
        227, 229, 233, 239, 241, 251, 257, 263,
        269, 271, 277, 281, 283, 293, 307, 311,
        313, 317, 331, 337, 347, 349, 353, 359,
        367, 373, 379, 383, 389, 397, 401, 409,
        419, 421, 431, 433, 439, 443, 449, 457,
        461, 463, 467, 479, 487, 491, 499, 503,
        509, 521, 523, 541};

int KeyedHashTable::Hash(const std::string &key) const
{
    int result = 0;
    for (int k = 0; k < key.length(); k++)
    {
        result += key[k] * PRIME_LIST[k];
    }
    return result % tableSize;
}

void KeyedHashTable::ReHash()
{
    int hash_result;
    int old_size = tableSize;
    int new_size = FindNearestLargerPrime(2 * tableSize);
    tableSize = new_size;
    HashData *new_table = new HashData[new_size];

    for (int k = 1; k < new_size; k++)
        new_table[k] = HashData();

    for (int i = 0; i < old_size; i++)
    {
        if (table[i].key != "__DELETED__")
        {
            hash_result = Hash(table[i].key);
            int tmp = hash_result;
            int cum_placeholder = 1;
            int helper_cum = 1;

            while (1)
            {
                if (new_table[hash_result].key == "")
                {
                    new_table[hash_result].key = table[i].key;
                    new_table[hash_result].intArray = table[i].intArray;
                    break;
                }
                else if (new_table[hash_result].key == table[i].key)
                    break;
                else
                {
                    hash_result = (tmp + cum_placeholder) % new_size;
                    helper_cum++;
                    cum_placeholder = helper_cum * helper_cum;
                }
            }
        }
    }

    delete[] table;
    table = new_table;
}

int KeyedHashTable::FindNearestLargerPrime(int requestedCapacity)
{
    for (int index = 0; index < PRIME_TABLE_COUNT; index++)
    {
        if (PRIME_LIST[index] > requestedCapacity)
            return PRIME_LIST[index];
    }
}

KeyedHashTable::KeyedHashTable()
{
    table = new HashData[2];
    for (int i = 0; i < 2; i++)
        table[i] = HashData();

    tableSize = 2;
    occupiedElementCount = 0;
}

KeyedHashTable::KeyedHashTable(int requestedCapacity)
{
    tableSize = FindNearestLargerPrime(requestedCapacity);
    table = new HashData[tableSize];
    for (int i = 0; i < tableSize; i++)
        table[i] = HashData();

    occupiedElementCount = 0;
}

KeyedHashTable::KeyedHashTable(const KeyedHashTable &other)
{
    table = new HashData[other.tableSize];
    tableSize = other.tableSize;
    occupiedElementCount = other.occupiedElementCount;

    for (int i = 0; i < other.tableSize; i++)
        table[i] = other.table[i];
}

KeyedHashTable &KeyedHashTable::operator=(const KeyedHashTable &other)
{
    if (this != &other)
    {
        delete[] table;
        table = new HashData[other.tableSize];
        tableSize = other.tableSize;
        occupiedElementCount = other.occupiedElementCount;

        for (int i = 0; i < other.tableSize; i++)
            table[i] = other.table[i];
    }
    return *this;
}

KeyedHashTable::~KeyedHashTable()
{
    delete[] table;
    tableSize = 0;
    occupiedElementCount = 0;
}

bool KeyedHashTable::Insert(const std::string &key, const std::vector<int> &intArray)
{
    int hash_result = Hash(key);
    int tmp = hash_result;
    int cum_placeholder = 1;
    int helper_cum = 1;

    while (1)
    {
        if (table[hash_result].key == "" || table[hash_result].key == "__DELETED__")
        {
            table[hash_result].key = key;
            table[hash_result].intArray = intArray;
            occupiedElementCount++;
            break;
        }
        else if (table[hash_result].key == key)
            return false;
        else
        {
            hash_result = (tmp + cum_placeholder) % tableSize;
            helper_cum++;
            cum_placeholder = helper_cum * helper_cum;
        }
    }

    if (occupiedElementCount * EXPAND_THRESHOLD >= tableSize)
        ReHash();

    return true;
}

bool KeyedHashTable::Remove(const std::string &key)
{
    std::vector<int> garbage;
    if (!Find(garbage, key))
        return false;

    int possible_hash = Hash(key);
    int tmp = possible_hash;
    int cum = 1;
    int helper_cum = 1;

    while (1)
    {
        if (table[possible_hash].key == key)
        {
            table[possible_hash] = HashData();
            table[possible_hash].key = "__DELETED__";
        }
        else if (table[possible_hash].key != key)
        {
            possible_hash = (tmp + cum) % tableSize;
            helper_cum++;
            cum = helper_cum * helper_cum;
        }
    }
}

void KeyedHashTable::ClearTable()
{
    for (int i = 0; i < tableSize; i++)
        if (table[i].key != "")
            table[i] = HashData();

    occupiedElementCount = 0;
}

bool KeyedHashTable::Find(std::vector<int> &valueOut, const std::string &key) const
{
    int possible_hash = Hash(key);
    int tmp = possible_hash;
    int cum = 1;
    int helper_cum = 1;

    while (1)
    {
        if (table[possible_hash].key == key)
        {
            valueOut = table[possible_hash].intArray;
            return true;
        }
        else if (table[possible_hash].key != "" && table[possible_hash].key != "__DELETED__")
        {
            possible_hash = (tmp + cum) % tableSize;
            helper_cum++;
            cum = helper_cum * helper_cum;
        }
        else
            return false;
    }
}

void KeyedHashTable::Print() const
{
    std::cout << "HT:";
    if (occupiedElementCount == 0)
    {
        std::cout << " Empty";
    }
    std::cout << "\n";
    for (int i = 0; i < tableSize; i++)
    {
        if (table[i].key == "")
            continue;

        std::cout << "[" << std::setw(3) << i << "] ";
        std::cout << "[" << table[i].key << "] ";
        std::cout << "[";
        for (size_t j = 0; j < table[i].intArray.size(); j++)
        {
            std::cout << table[i].intArray[j];
            if ((j + 1) != table[i].intArray.size())
                std::cout << ", ";
        }
        std::cout << "]\n";
    }
    std::cout.flush();
}