add_subdirectory(Event)
add_subdirectory(ImGui)
add_subdirectory(Layer)

set(HFILES "")
set(CPPFILES "")
file(GLOB HFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB CPPFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
RUNTIME_ADD_SOURCE(
        ${HFILES}
        ${CPPFILES}
)
