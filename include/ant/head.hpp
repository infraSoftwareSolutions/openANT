#pragma once

#include "cfrost/structure.h"

namespace ant
{
    ocf::hash_map<ocf::str, ocf::str> procedure;
    ocf::vec<uint8_t> ds;
    std::stack<uint8_t> ss;
    size_t command_counter = 0;
    size_t ptr = 0;
    bool stack_segment = false;
    bool is_loop = false;
    // Helper function to ensure memory capacity before accessing memory[ptr]
    void ensure_memory_capacity(size_t index)
    {
        while (ds.size() <= index)
            ds.push_back(0);
    }
}