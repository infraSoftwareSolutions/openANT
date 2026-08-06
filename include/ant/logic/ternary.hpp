#include "udt/basic/ternary.hpp"
#include "cfrost/structure.h"
#include <iostream>

namespace ant {
    class ternary
    {
    private:
        ocf::vec<udt::ternary> ternary_values;
        size_t ptr;
    public:
        ternary() = default;
        ~ternary() = default;
        ternary(const ocf::initList<udt::ternary> &values) : ternary_values(values), ptr(0) {}
        ternary(const ocf::vec<udt::ternary> &values) : ternary_values(values), ptr(0) {}
        ternary(const ocf::vec<udt::ternary> &values, size_t position) : ternary_values(values), ptr(position) {}
        ternary(const ternary &other) : ternary_values(other.ternary_values), ptr(other.ptr) {}

        constexpr procedure(reset) noexcept { ptr = 0; }

        constexpr procedure(clear) noexcept { ternary_values.clear(); }

        constexpr void operator<<(size_t value) noexcept {
            if(ptr == 0) return;
            ptr -= value;
        }

        constexpr void operator>>(size_t value) noexcept { ptr += value; }

        udt::ternary operator&&(udt::ternary value) noexcept {
            return ternary_values[ptr] && value;
        }

        udt::ternary operator||(udt::ternary value) noexcept {
            return ternary_values[ptr] || value;
        }

        udt::ternary operator^(udt::ternary value) noexcept {
            return ternary_values[ptr] ^ value;
        }

        udt::ternary operator!() noexcept {
            return !ternary_values[ptr];
        }

        udt::ternary operator~() noexcept {
            return ternary_values[ptr];
        }

        void operator=(udt::ternary value) noexcept {
            ternary_values[ptr] = value;
        }

        void operator=(const ternary &other) noexcept {
            ternary_values = other.ternary_values;
            ptr = other.ptr;
        }

        udt::ternary operator[](size_t index) {
            if(index >= ternary_values.size()) {
                std::cerr << "Index out of range" << newl;
                return udt::ternary(); // Return a default ternary value in case of out-of-range access
            }
            return ternary_values[index];
        }
    };
}