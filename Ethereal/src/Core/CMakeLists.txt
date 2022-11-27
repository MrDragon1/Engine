add_subdirectory(Animation)
add_subdirectory(Asset)
add_subdirectory(Project)
add_subdirectory(Renderer)
add_subdirectory(Scene)

set(HFILES "")
set(CPPFILES "")
file(GLOB HFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB CPPFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
RUNTIME_ADD_SOURCE(
        ${HFILES}
        ${CPPFILES}
)
