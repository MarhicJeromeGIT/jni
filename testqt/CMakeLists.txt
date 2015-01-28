
set (CMAKE_PREFIX_PATH 
	"C:\\Users\\XiongBaoBao\\Portfolio\\extern\\Qt\\qt5\\qtbase"
	#"C:\\Qt\\Qt5.2.0GL\\5.2.0\\msvc2012_64_opengl"
	"C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.0A\\Lib"
	)

# Qt library
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
set(CMAKE_AUTOMOC ON)

# OpenGL & GLEW library
find_package(OpenGL)

# executable
add_executable(qt5test main.cpp)
qt5_use_modules(qt5test Widgets OpenGL)
target_link_libraries(qt5test ${QT_LIBRARIES} ${OPENGL_LIBRARIES})