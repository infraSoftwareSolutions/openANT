#include "udt/core/bfs/bfs_8.hpp"
#include "cfrost/structure.h"
#include <exception>

namespace ant {
    class binary {
        private:
        ocf::vec<udt::bfs_8> binary_values;
        size_t ptr;
        protected:
        std::pair<size_t, uint8_t> real_location() {
            return std::pair(ptr / 8, ptr % 8);
        }
        public:
        binary() = default;
        ~binary() = default;
        
        void operator<<(size_t value) {
            if(ptr == 0) return;
            ptr -= value;
        }

        void operator>>(size_t value) {
            ptr += value;
        }

        bool operator&&(bool value) {
            auto location = real_location();
            return binary_values[location.first].get(location.second) && value;
        }

        bool operator||(bool value) {
            auto location = real_location();
            return binary_values[location.first].get(location.second) || value;
        }

        bool operator^(bool value) {
            auto location = real_location();
            return binary_values[location.first].get(location.second) ^ value;
        }
        
    };
}