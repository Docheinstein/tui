## TUI

**T**erminal **U**ser **I**nterface is a C++17 library that offers a minimal support for creating composable layouts for terminal interfaces.

This has been made to create friendly interface for the Debugger of [Docboy](https://github.com/Docheinstein/docboy/) (GameBoy emulator).

### Features

* Horizontal Layouts
* Vertical Layouts
* Blocks (automatic or fixed size)
* Colors

### Example

In you can find an usage example in `main.cpp`.

### Build

```
mkdir build
cmake ..
```


### Usage

To use TUI as a static library with CMake, copy `src` somewhere in your project directory and rename it to `tui`,
then add to your `CMakeLists.txt`:

```
add_executable(app STATIC)

...

add_subdirectory(tui)

target_link_libraries(app PRIVATE tui)
```