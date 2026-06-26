// C++23: std::mdspan (multi-dimensional array view)
// A non-owning view over contiguous storage with native multi-index operator[].
//
// NOTE: libstdc++ ships <mdspan> from GCC 15. To keep this sample buildable on
// GCC 14 we include the official reference implementation header (mdspan.hpp,
// vendored next to this file), which provides the same std::mdspan interface.
#include "mdspan.hpp"

#include <array>
#include <cstddef>
#include <iostream>

int main() {
    std::array<double, 12> storage{};
    // A 3x4 row-major view over the flat array.
    std::mdspan<double, std::extents<std::size_t, 3, 4>> m(storage.data());

    for (std::size_t i = 0; i < m.extent(0); ++i)
        for (std::size_t j = 0; j < m.extent(1); ++j)
            m[i, j] = static_cast<double>(i * 10 + j); // 2D indexing

    for (std::size_t i = 0; i < m.extent(0); ++i) {
        for (std::size_t j = 0; j < m.extent(1); ++j)
            std::cout << m[i, j] << '\t';
        std::cout << '\n';
    }
    return 0;
}
