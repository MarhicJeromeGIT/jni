
include_directories(
		${CMAKE_SOURCE_DIR} #for the config file 
		..\\CommonLibrary\\include
		..\\CommonLibrary\\
		..\\CommonLibrary\\Shaders
		..\\..\\extern\\assimp-3.1.1\\include # custom ASSIMP
		#"C:\\Qt\\Qt5.2.0GL\\5.2.0\\msvc2012\\include" # QT5		
		"C:\\Users\\XiongBaoBao\\Qt5Build\\qt5\\qtbase\\include" #QT
		"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\Visual Studio 2010\\Projects" # GLM
		"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\Visual Studio 2010\\Projects\\glew-1.7.0-win32\\glew-1.7.0\\include" #GLEW
		"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\boost_1_55_0\\boost_1_55_0" #BOOST
		"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\freetype-2.3.5-1-lib\\include" #FREETYPE
		"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\freetype-2.3.5-1-lib\\include\\freetype2" #FREETYPE
)

link_directories(
	#"C:\\Qt\\Qt5.2.0GL\\5.2.0\\msvc2012\\lib"
	"C:\\Users\\XiongBaoBao\\Qt5Build\\qt5\\qtbase\\lib" #QT
	"C:\\Users\\XiongBaoBao\\Portfolio\\extern\\assimp-3.1.1\\lib\\Release" #ASSIMP
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\SFML-2.1-windows-vc10-32bits\\SFML-2.1\\lib"  # SFML
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\Visual Studio 2010\\Projects\\glew-1.7.0-win32\\glew-1.7.0\\lib" # GLEW
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\boost_1_55_0\\boost_1_55_0\\bin.v2\\libs\\system\\build\\msvc-11.0\\debug\\link-static\\threading-multi" # BOOST
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\boost_1_55_0\\boost_1_55_0\\bin.v2\\libs\\filesystem\\build\\msvc-11.0\\debug\\link-static\\threading-multi" # BOOST
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\boost_1_55_0\\boost_1_55_0\\bin.v2\\libs\\date_time\\build\\msvc-11.0\\debug\\link-static\\threading-multi" #boost
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\boost_1_55_0\\boost_1_55_0\\bin.v2\\libs\\regex\\build\\msvc-11.0\\debug\\link-static\\threading-multi"	
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\freetype-2.3.5-1-lib\\lib" #freetype2

	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\boost_1_55_0\\boost_1_55_0\\bin.v2\\libs\\system\\build\\msvc-11.0\\release\\link-static\\threading-multi" # BOOST
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\boost_1_55_0\\boost_1_55_0\\bin.v2\\libs\\filesystem\\build\\msvc-11.0\\release\\link-static\\threading-multi" # BOOST
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\boost_1_55_0\\boost_1_55_0\\bin.v2\\libs\\date_time\\build\\msvc-11.0\\release\\link-static\\threading-multi" #boost
	"C:\\Users\\XiongBaoBao\\Looper 2012 DVDRip AC3 XViD-RemixHD\\Documents\\boost_1_55_0\\boost_1_55_0\\bin.v2\\libs\\regex\\build\\msvc-11.0\\release\\link-static\\threading-multi"	
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.   
set(CMAKE_AUTOMOC ON)

set (CMAKE_PREFIX_PATH 
	"C:\\Users\\XiongBaoBao\\Portfolio\\extern\\Qt\\qt5\\qtbase"
	#"C:\\Qt\\Qt5.2.0GL\\5.2.0\\msvc2012_64_opengl"
	"C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.0A\\Lib"
	)


# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)

#liste des fichiers du projet
file(GLOB_RECURSE
     source_files
     *.cpp 
)
file(GLOB_RECURSE
     header_files
	 *.h
)

SET(helloworld_SOURCES ${source_files} )
SET(helloworld_HEADERS ${header_files} )
QT5_WRAP_CPP(helloworld_HEADERS_MOC ${helloworld_HEADERS})

# Tell CMake to create the helloworld executable
add_executable(SceneEditor ${helloworld_SOURCES} 
						  
						   ${helloworld_HEADERS}	 )

SET(BOOST_SYSTEM_LIB optimized libboost_system-vc110-mt-1_55 debug libboost_system-vc110-mt-gd-1_55)
SET(BOOST_FILESYSTEM_LIB optimized libboost_filesystem-vc110-mt-1_55 debug libboost_filesystem-vc110-mt-gd-1_55 )
SET(WINSOCK_LIB optimized WSock32 debug WSock32 )
SET(SFML_AUDIO optimized sfml-audio debug sfml-audio-d)

# Use the Widgets module from Qt 5.
qt5_use_modules(SceneEditor Core Gui Widgets OpenGL)

target_link_libraries(
		SceneEditor
 		Shlwapi
 		CommonLibrary
		assimp
		GLEW32
		opengl32
		libfftw3-3
	 	${SFML_AUDIO}
		${BOOST_SYSTEM_LIB}
		${BOOST_FILESYSTEM_LIB}
		${WINSOCK_LIB}
)
