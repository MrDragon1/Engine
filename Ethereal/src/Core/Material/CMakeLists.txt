add_subdirectory(ShaderGenerator)
add_subdirectory(MaterialBase)
set(HFILES "")
set(CPPFILES "")
file(GLOB HFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB CPPFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
RUNTIME_ADD_SOURCE(
        ${HFILES}
        ${CPPFILES}
)
