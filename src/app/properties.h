#pragma once

#include <string>

#include <QWidget>

namespace cartan::render {
class ScalarQuantity;
class VectorQuantity;
} // namespace cartan::render

namespace cartan::app {

class Context;

class Properties : public QWidget {
public:
  explicit Properties(Context &context, QWidget *parent = nullptr);

  void select(const std::string &structure, const std::string &quantity);

private:
  void setContent(QWidget *content);
  QWidget *scalarControls(render::ScalarQuantity &quantity);
  QWidget *vectorControls(render::VectorQuantity &quantity);

  render::ScalarQuantity *scalar() const;
  render::VectorQuantity *vector() const;
  void changed();

  Context &m_context;
  std::string m_structure;
  std::string m_quantity;
  QWidget *m_content = nullptr;
};

} // namespace cartan::app
