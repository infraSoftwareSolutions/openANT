#include "cfrost/structure.h"
#include <cstdint>
#include <iostream>

class ant
{
private:
    ocf::vec<uint8_t> memory;
    size_t ptr;

protected:
    void ensure_memory_capacity(size_t index)
    {
        while (memory.size() <= index)
        {
            memory.push_back(0);
        }
    }

public:
    ant() = default;
    ~ant() = default;
    ant(size_t value) noexcept : ptr(value)
    {
        for (iter_less(i, value))
        {
            memory.push_back(0);
        }
    }
    ant(const ocf::vec<uint8_t> &list) noexcept : memory(list), ptr(0) {}
    ant(const ant &other) noexcept : memory(other.memory), ptr(other.ptr) {}
    ant(ocf::initList<uint8_t> list) noexcept
    {
        for (uint8_t value : list)
        {
            memory.push_back(value);
            ptr++;
        }
    }
    void operator<<(size_t index) noexcept
    {
        if (ptr == 0)
            return;
        ptr -= index;
    }
    void operator>>(size_t index) noexcept
    {
        ptr += index;
        ensure_memory_capacity(ptr);
    }
    procedure(operator++) noexcept { memory[ptr]++; }
    procedure(operator--) noexcept { memory[ptr]--; }
    uint8_t operator+(uint8_t value) noexcept { return memory[ptr] + value; }
    uint8_t operator-(uint8_t value) noexcept { return memory[ptr] - value; }
    uint8_t operator*(uint8_t value) noexcept { return memory[ptr] * value; }
    uint8_t operator/(uint8_t value) noexcept { return memory[ptr] / value; }
    uint8_t operator%(uint8_t value) noexcept { return memory[ptr] % value; }
    void operator+=(uint8_t value) noexcept { memory[ptr] += value; }
    void operator-=(uint8_t value) noexcept { memory[ptr] -= value; }
    void operator*=(uint8_t value) noexcept { memory[ptr] *= value; }
    void operator/=(uint8_t value) noexcept { memory[ptr] /= value; }
    void operator%=(uint8_t value) noexcept { memory[ptr] %= value; }
    procedure(operator!) noexcept { memory[ptr] = !memory[ptr]; }
    uint8_t operator||(uint8_t value) noexcept { return memory[ptr] || value; }
    uint8_t operator&&(uint8_t value) noexcept { return memory[ptr] && value; }
    uint8_t operator^(uint8_t value) noexcept { return memory[ptr] ^ value; }
    void operator|=(uint8_t value) noexcept { memory[ptr] |= value; }
    void operator&=(uint8_t value) noexcept { memory[ptr] &= value; }
    void operator^=(uint8_t value) noexcept { memory[ptr] ^= value; }
    uint8_t &operator[](size_t index) noexcept { return memory[index]; }
    procedure(print) noexcept
    {
        std::cout
            << memory[ptr]
            << newl;
    }
    procedure(print_char) noexcept
    {
        std::cout
            << (char)memory[ptr]
            << newl;
    }
};