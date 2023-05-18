# CMakeList.txt : CMake project for EPANET_Cmake, include source and define
# project specific logic here.
#
IF(WIN32)
  add_library(epanet SHARED IMPORTED )
  set_property(TARGET epanet PROPERTY
              IMPORTED_LOCATION "${EPANET_ROOT}/build/bin/Release/epanet2.dll")
  set_property(TARGET epanet PROPERTY
             IMPORTED_IMPLIB "${EPANET_ROOT}/build/lib/Release/epanet2.lib")
ENDIF(WIN32)

IF(APPLE)
add_library(epanet SHARED IMPORTED)
set_target_properties(epanet PROPERTIES
            IMPORTED_LOCATION "${EPANET_ROOT}/build/lib/libepanet2.dylib" )
ENDIF(APPLE)

#add_library(epanet OBJECT "D:/repos/EPANET/build/lib/Release/epanet2.exp")
# Add source to this project's executable.
add_executable (epanet_cmake "EPANET_Cmake.cpp" "EPANET_Cmake.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET epanet_cmake PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
target_link_libraries( epanet_cmake PUBLIC epanet )

target_include_directories(epanet_cmake PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${EPANET_ROOT}/include"
                          )