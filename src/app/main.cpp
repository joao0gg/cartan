#include <utility>
#include <vector>

#include <QApplication>
#include <QSurfaceFormat>

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
  }

  cartan::app::MainWindow window(context);
  window.show();

  return app.exec();
}