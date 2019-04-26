include_directories(${Praktikum_SOURCE_DIR}/common 
                    ${Praktikum_SOURCE_DIR}/external/inc
                    )

file(GLOB COMMON_SRC 
  "${Praktikum_SOURCE_DIR}/common/*.h"
  "${Praktikum_SOURCE_DIR}/common/*.cpp"
)
file(GLOB praktikum_SRC
    "*.h"
    "*.cpp"
)

add_executable(defTrans ${praktikum_SRC} ${COMMON_SRC})
find_package(OpenGL REQUIRED)
find_package (Eigen3 REQUIRED)
find_package(OpenMP)
include_directories( ${OPENGL_INCLUDE_DIRS})
link_directories(${Praktikum_SOURCE_DIR}/external/lib)
target_link_libraries(defTrans 
                      ${OPENGL_LIBRARIES}
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
                       Eigen3::Eigen
                       OpenMP::OpenMP_CXX
                       )