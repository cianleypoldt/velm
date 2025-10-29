#include <algorithm>
#include <span>
#include <vector>

namespace velm_dr {

struct mesh {
    std::span<float> vertices;
    std::span<float> indices;
};
}  // namespace velm_dr
