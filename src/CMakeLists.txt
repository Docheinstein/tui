add_library(tui STATIC)

target_sources(tui PUBLIC
    block.cpp
    container.cpp
    decorators.cpp
    factory.cpp
    presenter.cpp
    text.cpp
)
