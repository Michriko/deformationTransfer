include_directories(${Praktikum_SOURCE_DIR}/common)

file(GLOB COMMON_SRC 
  "${Praktikum_SOURCE_DIR}/common/*.h"
  "${Praktikum_SOURCE_DIR}/common/*.cpp"
)
file(GLOB praktikum_SRC
    "*.h"
    "*.cpp"
)

message(${praktikum_SRC})
message (${COMMON_SRC})
add_executable(Corres ${praktikum_SRC} ${COMMON_SRC})
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package( glm REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(Corres ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )