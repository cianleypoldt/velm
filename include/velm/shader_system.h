#include "bgfx/bgfx.h"

#include <string_view>
#include <vector>

namespace velm_shadersys {

void load_all();
void destroy_all();

bgfx::ShaderHandle retrieve(std::string_view);

namespace {
std::vector<std::pair<std::string_view, bgfx::ShaderHandle>> shaders;
}

};  // namespace velm_shadersys
