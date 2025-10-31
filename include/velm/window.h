#pragma once

namespace velm {

class window {
    using glfw_handle = void *;

  public:
    window(int width, int height, char * title);
    ~window();

    void attach_bgfx_context();
  private:
    glfw_handle handle;
};
};  // namespace velm
