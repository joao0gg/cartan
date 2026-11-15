#include "app/colorbardialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QPushButton>
#include <QSpinBox>

#include "app/context.h"

namespace cartan::app {

ColorBarDialog::ColorBarDialog(Context &context, QWidget *parent)
    : QDialog(parent), m_context(context), m_original(context.colorBarStyle()) {
  setWindowTitle("Color Bar Settings");

  m_width = new QSpinBox;
  m_width->setRange(kMinColorBarWidth, kMaxColorBarWidth);
  m_width->setSuffix(" px");
  m_width->setValue(m_original.width);

  m_height = new QSpinBox;
  m_height->setRange(kMinColorBarHeight, kMaxColorBarHeight);
  m_height->setSuffix(" px");
  m_height->setValue(m_original.height);

  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel |
                                       QDialogButtonBox::RestoreDefaults);

  auto *layout = new QFormLayout(this);
  layout->addRow("Width", m_width);
  layout->addRow("Height", m_height);
  layout->addRow(buttons);

  connect(m_width, &QSpinBox::valueChanged, this, &ColorBarDialog::preview);
  connect(m_height, &QSpinBox::valueChanged, this, &ColorBarDialog::preview);

  connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
    m_context.setColorBarStyle({m_width->value(), m_height->value()}, true);
    accept();
  });

  connect(buttons, &QDialogButtonBox::rejected, this, [this]() {
    m_context.setColorBarStyle(m_original, false);
    reject();
  });

  connect(buttons->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this,
          [this]() {
            const ColorBarStyle defaults;
            m_width->setValue(defaults.width);
            m_height->setValue(defaults.height);
          });
}

void ColorBarDialog::preview() {
  m_context.setColorBarStyle({m_width->value(), m_height->value()}, false);
}

} // namespace cartan::app
