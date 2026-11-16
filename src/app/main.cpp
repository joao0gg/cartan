#include <utility>
#include <vector>

#include <QApplication>
#include <QSurfaceFormat>

#include <glm/geometric.hpp>

#include "app/MainWindow.h"
#include "app/context.h"

int main(int argc, char **argv) {
  QSurfaceFormat format;
  format.setVersion(4, 1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize(24);
  format.setSamples(4);
  QSurfaceFormat::setDefaultFormat(format);

  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

  QApplication app(argc, argv);
  app.setApplicationName("Cartan");

  cartan::app::Context context;
  context.log().install();
  context.restoreTheme();

  auto *mesh = context.openMesh("/home/joao/Desktop/cartan/assets/torus.obj");

  if (mesh != nullptr) {
    std::vector<float> height;
    for (const glm::vec3 &p : mesh->positions()) {
      height.push_back(p.y);
    }

    mesh->addVertexScalar("height", std::move(height));
    mesh->setActiveQuantity("height");

    std::vector<glm::vec3> swirl;
    for (const glm::vec3 &p : mesh->positions()) {
      swirl.emplace_back(-p.z, 0.0f, p.x);
    }

    mesh->addVertexVector("swirl", std::move(swirl));

    std::vector<glm::vec3> normals;
    for (const auto &[a, b, c] : mesh->triangles()) {
      const glm::vec3 &pa = mesh->positions()[a];
      const glm::vec3 &pb = mesh->positions()[b];
      const glm::vec3 &pc = mesh->positions()[c];

      normals.push_back(glm::normalize(glm::cross(pb - pa, pc - pa)));
    }

    auto &faceNormals = mesh->addFaceVector("face normals", std::move(normals));
    faceNormals.options.color = {0.0f, 0.0f, 1.0f};
    faceNormals.options.enabled = false;
  }

  cartan::app::MainWindow window(context);
  window.show();

  return app.exec();
}