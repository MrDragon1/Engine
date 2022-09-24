add_subdirectory(Animation)
add_subdirectory(Asset)
add_subdirectory(Core)
add_subdirectory(Event)
add_subdirectory(ImGui)
add_subdirectory(Renderer)
add_subdirectory(Scene)
add_subdirectory(Utils)
add_subdirectory(Platform)
add_subdirectory(Project)

set(HFILES "")
set(CPPFILES "")
file(GLOB HFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB CPPFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
RUNTIME_ADD_SOURCE(
        ${HFILES}
        ${CPPFILES}
)
