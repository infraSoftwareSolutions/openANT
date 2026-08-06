#include "cfrost/cgen/ternary.h"
#include "cfrost/structure.h"

#pragma once
#ifndef BCT_HPP
#define BCT_HPP

namespace ant {
    class bct {
        private:
        ocf::vec<tirt> tirt_value;
        ocf::vec<utirt> utirt_value;
        size_t ptr;
        bool singed = true;
        public:
        bct() = default;
        ~bct() = default;
    };
}

#endif // BCT_HPP