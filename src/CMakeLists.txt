add_library(shrink INTERFACE)
target_include_directories(shrink
INTERFACE
    .
)
target_link_libraries(shrink
INTERFACE
    fassert
)
