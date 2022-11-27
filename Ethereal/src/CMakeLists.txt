add_subdirectory(Base)
add_subdirectory(Core)
add_subdirectory(Utils)
add_subdirectory(Platform)
add_subdirectory(Resource)


set(HFILES "")
set(CPPFILES "")
file(GLOB HFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB CPPFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
RUNTIME_ADD_SOURCE(
        ${HFILES}
        ${CPPFILES}
)
