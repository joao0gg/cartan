![Cartan](img/cartan-logo-light.svg)

# Cartan

Cartan is a multiphysics engine based on finite element exterior calculus (FEEC), named after the French mathematician Élie Cartan.

## Building

Requires CMake ≥ 3.20, C++20, Qt 6 (Core, OpenGL, Widgets, OpenGLWidgets), assimp, glm.

```sh
cmake -S . -B build
cmake --build build
```

### Running

```sh
./build/cartan
```

### Testing

```sh
cd build && ctest --output-on-failure
```

## Layers

Cartan follows a layered architecture. Each layer is a CMake target, and the public API is in the `cartan::<layer>` namespace. The layers are:

| Directory | Target | Namespace | Links |
| --- | --- | --- | --- |
| `src/core` | `cartan_core` | `cartan::core` | glm |
| `src/io` | `cartan_io` | `cartan::io` | core, assimp (private) |
| `src/app` | `cartan_app` | `cartan::app` | core |
| `src/render` | `cartan_render` | `cartan::render` | core, Qt6::OpenGL |
| `src/gui` | `cartan_gui` | `cartan::gui` | app, Qt6::Widgets |
| `src/modules/<name>` | `cartan_module_<name>` | per module | gui + declared |

TODO: add an architecture diagram.
TODO: document the public API of each layer.
TODO: document the public API of each module.
TODO: document the public API of the engine as a whole.
