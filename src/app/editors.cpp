#include "app/editors.h"

#include "app/viewport.h"

namespace cartan::app {

const QList<EditorType> &editorTypes() {
  static const QList<EditorType> types = {
      {"Viewport",
       [](Context &context) {
         return new Viewport(context);
       }},
  };

  return types;
}

} // namespace cartan::app
