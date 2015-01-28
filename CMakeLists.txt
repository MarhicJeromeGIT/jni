# ubuntu
#cmake ../Antigravity/ -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug

# windows
#cmake ../jni -G "Visual Studio 11"

cmake_minimum_required (VERSION 2.6)

set(GCC_COVERAGE_COMPILE_FLAGS "-std=gnu++0x")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
#ADD_DEFINITIONS(-D_SECURE_SCL=0)
#ADD_DEFINITIONS(-D_HAS_ITERATOR_DEBUGGING=0)

project (AlienSurf)

add_subdirectory (CommonLibrary)
add_subdirectory(SceneEditor)
add_subdirectory (testqt)

message( "coucou" )
message( "${CMAKE_BUILD_TYPE}" )



