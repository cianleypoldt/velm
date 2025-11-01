#pragma once

#include "velm/scene.h"
#include "velm/shader_system.h"

#include <memory>

namespace velm {
class Velm {
  public:
    Velm();
    ~Velm();
    void          drop();
    velm::scene * create_scene();
  private:
    std::vector<std::weak_ptr<scene>> scenes;
};

}  // namespace velm
