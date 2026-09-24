#pragma once

#include <QObject>

#include <string>
#include <vector>

#include "core/Mesh.h"

namespace cartan::core {

struct SceneObject {
  std::string name;
  Mesh mesh;
};

class Scene : public QObject {
  Q_OBJECT

public:
  void add(std::string name, Mesh mesh);
  void clear();

  const std::vector<SceneObject> &objects() const {
    return m_objects;
  }

signals:
  void changed();

private:
  std::vector<SceneObject> m_objects;
};

} // namespace cartan::core
