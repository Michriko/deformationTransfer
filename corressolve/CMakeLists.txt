include_directories(${Praktikum_SOURCE_DIR}/common 
                    ${Praktikum_SOURCE_DIR}/external/inc
                    ${Praktikum_SOURCE_DIR}/external/inc/pcl
                    )

file(GLOB COMMON_SRC 
  "${Praktikum_SOURCE_DIR}/common/*.h"
  "${Praktikum_SOURCE_DIR}/common/*.cpp"
)
file(GLOB praktikum_SRC
    "*.h"
    "*.cpp"
)
file(GLOB LIBPCL 
    "${Praktikum_SOURCE_DIR}/external/lib/pcl/*.so"
)

add_executable(corressolver ${praktikum_SRC} ${COMMON_SRC})
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package (Threads REQUIRED)
find_package (Eigen3 REQUIRED)
find_package (PCL REQUIRED)
find_package(OpenMP)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${PCL_INCLUDE_DIRS})
link_directories(${Praktikum_SOURCE_DIR}/external/lib ${PCL_LIBRARY_DIRS})
target_link_libraries(corressolver 
                      ${OPENGL_LIBRARIES}
                       ${GLUT_LIBRARY} 
                       ${CMAKE_THREAD_LIBS_INIT} 
                       ${Praktikum_SOURCE_DIR}/external/lib/libumfpack.a
                       ${Praktikum_SOURCE_DIR}/external/lib/liblapack.a
                       ${Praktikum_SOURCE_DIR}/external/lib/libblas.a
                       ${Praktikum_SOURCE_DIR}/external/lib/libcholmod.a 
                       ${Praktikum_SOURCE_DIR}/external/lib/libamd.a
                       ${Praktikum_SOURCE_DIR}/external/lib/libcamd.a
                       ${Praktikum_SOURCE_DIR}/external/lib/libccolamd.a
                       ${Praktikum_SOURCE_DIR}/external/lib/libcolamd.a
                       ${Praktikum_SOURCE_DIR}/external/lib/libsuitesparseconfig.a
                       ${Praktikum_SOURCE_DIR}/external/lib/libmetis.so.5.1.0
                       ${Praktikum_SOURCE_DIR}/external/lib/liblapack.a
                       ${Praktikum_SOURCE_DIR}/external/lib/libblas.a
                       ${PCL_LIBRARIES}
                       Eigen3::Eigen
                       OpenMP::OpenMP_CXX
                       )