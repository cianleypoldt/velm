#pragma once

#include "velm/scene.h"

#include <memory>

namespace velm {
class Velm {
  public:
    Velm();
    ~Velm();

    [[nodiscard]] std::shared_ptr<Scene> create_scene();
  private:
    std::vector<std::weak_ptr<Scene>> scenes;
};

}  // namespace velm
