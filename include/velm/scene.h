#pragma once
#include <bgfx/bgfx.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace velm_render {

class view_component {};

class opaque_mesh : view_component {};

class transparent_mesh : view_component {};

class transparent_effect : view_component {};

class view {
    std::vector<view_component *> view_components;
    glm::mat4x4                   view_mat;
    bgfx::TextureHandle           render_target;
    bgfx::TextureHandle           depth_buffer_target;

  public:
    view();
    ~view();
    void render();
    void add_component();
};

struct mesh {
    enum class type : char { OPAQUE, TRANSPARENT };
    enum class buffer_type : char { STATIC, DYNAMIC };
    glm::mat4x4 transform;
};
}  // namespace velm_render

namespace velm {
class scene {
    std::vector<velm_render::view>   views;
    std::vector<velm_render::mesh *> meshes;
  public:
    scene();
    ~scene();
    void add_mesh();
    void add_view();
};
}  // namespace velm
