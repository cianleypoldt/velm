#include "bgfx/bgfx.h"

#include <string>
#include <vector>

namespace velm_shadersys {

void load_all();
void destroy_all();

[[nodiscard]] bgfx::ShaderHandle retrieve(std::string_view);

namespace {
std::vector<std::pair<std::string, bgfx::ShaderHandle>> shaders;
}

};  // namespace velm_shadersys
