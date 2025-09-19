add_subdirectory(fancy_assert/src)
target_compile_definitions(fassert
PUBLIC
    ENABLE_STD_FORMAT)

add_subdirectory(testa/cpp)
target_compile_definitions(testa
PUBLIC
    ENABLE_STD_FORMAT)
