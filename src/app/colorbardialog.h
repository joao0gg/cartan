#pragma once

#include <QDialog>

#include "app/colorbar.h"

class QSpinBox;

namespace cartan::app {

class Context;

class ColorBarDialog : public QDialog {
public:
  explicit ColorBarDialog(Context &context, QWidget *parent = nullptr);

private:
  void preview();

  Context &m_context;
  ColorBarStyle m_original;
  QSpinBox *m_width = nullptr;
  QSpinBox *m_height = nullptr;
};

} // namespace cartan::app
