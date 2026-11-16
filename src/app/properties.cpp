#include "app/properties.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#include "app/colorbar.h"
#include "app/context.h"

namespace cartan::app {

namespace {

constexpr double kMaxValue = 1e12;
constexpr int kValueDecimals = 6;

constexpr double kMinLengthScale = 0.01;
constexpr double kMaxLengthScale = 100.0;
constexpr double kMinRadius = 0.001;
constexpr double kMaxRadius = 1.0;

QDoubleSpinBox *doubleBox(double min, double max, double step, int decimals, double value) {
  auto *box = new QDoubleSpinBox;
  box->setRange(min, max);
  box->setSingleStep(step);
  box->setDecimals(decimals);
  box->setValue(value);

  return box;
}

QSpinBox *intBox(int min, int max, int value) {
  auto *box = new QSpinBox;
  box->setRange(min, max);
  box->setSuffix(" px");
  box->setValue(value);

  return box;
}

} // namespace

Properties::Properties(Context &context, QWidget *parent) : QWidget(parent), m_context(context) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  select({}, {});
}

void Properties::select(const std::string &structure, const std::string &quantity) {
  m_structure = structure;
  m_quantity = quantity;

  if (render::ScalarQuantity *found = scalar()) {
    setContent(scalarControls(*found));
  } else if (render::VectorQuantity *found = vector()) {
    setContent(vectorControls(*found));
  } else {
    auto *label = new QLabel("Select a quantity to edit it.");
    label->setAlignment(Qt::AlignCenter);
    label->setEnabled(false);
    setContent(label);
  }
}

void Properties::setContent(QWidget *content) {
  if (m_content != nullptr) {
    layout()->removeWidget(m_content);
    m_content->deleteLater();
  }

  m_content = content;
  layout()->addWidget(m_content);
}

QWidget *Properties::scalarControls(render::ScalarQuantity &quantity) {
  const render::ScalarOptions &options = quantity.options;
  const auto [min, max] = quantity.range();

  auto *colormap = new QComboBox;

  for (const render::Colormap map : render::kColormaps) {
    colormap->addItem(QString::fromUtf8(render::colormapName(map).data()), int(map));
  }

  colormap->setCurrentIndex(colormap->findData(int(options.colormap)));

  auto *autoRange = new QCheckBox("Automatic");
  autoRange->setChecked(options.autoRange);

  auto *minBox = doubleBox(-kMaxValue, kMaxValue, 0.1, kValueDecimals, min);
  auto *maxBox = doubleBox(-kMaxValue, kMaxValue, 0.1, kValueDecimals, max);
  minBox->setEnabled(!options.autoRange);
  maxBox->setEnabled(!options.autoRange);

  auto *showBar = new QCheckBox("Show");
  showBar->setChecked(options.colorBar.visible);

  auto *width = intBox(kMinColorBarWidth, kMaxColorBarWidth, options.colorBar.width);
  auto *height = intBox(kMinColorBarHeight, kMaxColorBarHeight, options.colorBar.height);

  auto *mapGroup = new QGroupBox("Color map");
  auto *mapLayout = new QFormLayout(mapGroup);
  mapLayout->addRow("Map", colormap);
  mapLayout->addRow("Range", autoRange);
  mapLayout->addRow("Min", minBox);
  mapLayout->addRow("Max", maxBox);

  auto *barGroup = new QGroupBox("Color bar");
  auto *barLayout = new QFormLayout(barGroup);
  barLayout->addRow("", showBar);
  barLayout->addRow("Width", width);
  barLayout->addRow("Height", height);

  connect(colormap, &QComboBox::currentIndexChanged, this, [this, colormap]() {
    if (render::ScalarQuantity *found = scalar()) {
      found->options.colormap = render::Colormap(colormap->currentData().toInt());
      changed();
    }
  });

  connect(autoRange, &QCheckBox::toggled, this, [this, minBox, maxBox](bool checked) {
    render::ScalarQuantity *found = scalar();

    if (found == nullptr) {
      return;
    }

    if (!checked) {
      const auto [dataMin, dataMax] = found->dataRange();
      found->options.min = dataMin;
      found->options.max = dataMax;
      minBox->setValue(dataMin);
      maxBox->setValue(dataMax);
    }

    found->options.autoRange = checked;
    minBox->setEnabled(!checked);
    maxBox->setEnabled(!checked);
    changed();
  });

  connect(minBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
    if (render::ScalarQuantity *found = scalar(); found != nullptr && !found->options.autoRange) {
      found->options.min = float(value);
      changed();
    }
  });

  connect(maxBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
    if (render::ScalarQuantity *found = scalar(); found != nullptr && !found->options.autoRange) {
      found->options.max = float(value);
      changed();
    }
  });

  connect(showBar, &QCheckBox::toggled, this, [this](bool checked) {
    if (render::ScalarQuantity *found = scalar()) {
      found->options.colorBar.visible = checked;
      changed();
    }
  });

  connect(width, &QSpinBox::valueChanged, this, [this](int value) {
    if (render::ScalarQuantity *found = scalar()) {
      found->options.colorBar.width = value;
      changed();
    }
  });

  connect(height, &QSpinBox::valueChanged, this, [this](int value) {
    if (render::ScalarQuantity *found = scalar()) {
      found->options.colorBar.height = value;
      changed();
    }
  });

  auto *content = new QWidget;
  auto *layout = new QVBoxLayout(content);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mapGroup);
  layout->addWidget(barGroup);
  layout->addStretch();

  return content;
}

QWidget *Properties::vectorControls(render::VectorQuantity &quantity) {
  const render::VectorOptions &options = quantity.options;

  auto *autoScale = new QCheckBox("Automatic");
  autoScale->setChecked(options.autoScale);

  auto *lengthScale =
      doubleBox(kMinLengthScale, kMaxLengthScale, 0.1, 3, double(options.lengthScale));
  auto *radius = doubleBox(kMinRadius, kMaxRadius, 0.01, 3, double(options.radius));

  auto *group = new QGroupBox("Arrows");
  auto *form = new QFormLayout(group);
  form->addRow("Length", autoScale);
  form->addRow("Scale", lengthScale);
  form->addRow("Radius", radius);

  connect(autoScale, &QCheckBox::toggled, this, [this](bool checked) {
    if (render::VectorQuantity *found = vector()) {
      found->options.autoScale = checked;
      changed();
    }
  });

  connect(lengthScale, &QDoubleSpinBox::valueChanged, this, [this](double value) {
    if (render::VectorQuantity *found = vector()) {
      found->options.lengthScale = float(value);
      changed();
    }
  });

  connect(radius, &QDoubleSpinBox::valueChanged, this, [this](double value) {
    if (render::VectorQuantity *found = vector()) {
      found->options.radius = float(value);
      changed();
    }
  });

  auto *content = new QWidget;
  auto *layout = new QVBoxLayout(content);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(group);
  layout->addStretch();

  return content;
}

render::ScalarQuantity *Properties::scalar() const {
  auto *mesh = dynamic_cast<render::SurfaceMesh *>(m_context.scene().find(m_structure));

  return mesh != nullptr && !m_quantity.empty() ? mesh->findQuantity(m_quantity) : nullptr;
}

render::VectorQuantity *Properties::vector() const {
  auto *mesh = dynamic_cast<render::SurfaceMesh *>(m_context.scene().find(m_structure));

  return mesh != nullptr && !m_quantity.empty() ? mesh->findVectorQuantity(m_quantity) : nullptr;
}

void Properties::changed() {
  m_context.notifyDisplayChanged();
}

} // namespace cartan::app
