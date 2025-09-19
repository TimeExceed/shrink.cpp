file(GLOB cpps *.cpp)
add_executable(shrink.unittest ${cpps})
target_link_libraries(shrink.unittest
PRIVATE
    shrink
    testa
)
