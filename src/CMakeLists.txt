add_library(tui STATIC)

target_sources(tui PRIVATE
    block.cpp
    block.h
    container.cpp
    container.h
    decorator.cpp
    decorator.h
    decorators.cpp
    decorators.tpp
    hlayout.h
    node.h
    presenter.cpp
    presenter.h
    text.cpp
    text.h
    token.cpp
    token.h
    vlayout.h
)

target_include_directories(tui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

