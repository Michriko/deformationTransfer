include_directories(${Praktikum_SOURCE_DIR}/common 
                    ${Praktikum_SOURCE_DIR}/external/inc)
link_directories(${Praktikum_SOURCE_DIR}/external/lib)
file(GLOB COMMON_SRC 
  "${Praktikum_SOURCE_DIR}/common/*.h"
  "${Praktikum_SOURCE_DIR}/common/*.cpp"
)
file(GLOB praktikum_SRC
    "*.h"
    "*.cpp"
)
add_executable(adjCalc ${praktikum_SRC} ${COMMON_SRC})
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package (Threads REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
target_link_libraries(adjCalc ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})