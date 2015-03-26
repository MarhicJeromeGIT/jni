cmake_minimum_required(VERSION 2.6)

set(GCC_COVERAGE_COMPILE_FLAGS "-std=gnu++0x")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

include_directories(
	include
	Shaders
	..\\..\\extern\\assimp-3.1.1\\include
	"C:\\Qt\\Qt5.2.0GL\\5.2.0\\msvc2012\\include" # QT5		
	${CMAKE_SOURCE_DIR} #for the config file 
	.
	"C:\\Users\\XiongBaoBao\\quelmerdier\\Documents\\SFML-2.1-windows-vc10-32bits\\SFML-2.1\\include"  # SFML
	..\\..\\extern # GLM
	..\\..\\extern\\glm
	..\\..\\extern\\glew-1.7.0-win32\\glew-1.7.0\\include #GLEW
	#"C:\\Users\\XiongBaoBao\\quelmerdier\\Documents\\boost_1_55_0\\boost_1_55_0" #BOOST
	..\\..\\extern\\boost_1_57_0\\boost_1_57_0\\boost
	..\\..\\extern\\boost_1_57_0\\boost_1_57_0
	"C:\\Users\\XiongBaoBao\\quelmerdier\\Documents\\freetype-2.3.5-1-lib\\include" #FREETYPE
	"C:\\Users\\XiongBaoBao\\quelmerdier\\Documents\\freetype-2.3.5-1-lib\\include\\freetype2" #FREETYPE

)

link_directories(
#	"C:/Users/XiongBaoBao/0101GravityBang/jni/CommonLibrary/build_vs2012/code/Debug"
#	"/home/jerome/Téléchargements/boost_1_55_0/~/boost/install/lib/"
#	"Assimp/lib"
#	"/usr/lib/gcc4/x64/debug"
#	"/usr/lib" # for SFML
)

file(GLOB_RECURSE
		 source_files
		 src/*
		 include/*
		 Shaders/*.cpp
		 Shaders/*.h
		 Gui/*.cpp
		 CommonLibrary.h
)

#STATIC, pour générer une bibliothèque à lier statiquement ; (par defaut)
#SHARED, pour générer une bibliothèque à lier dynamiquement.
add_library(
		CommonLibrary
		STATIC
		${source_files}
)

#Configuration de l'édition de liens
target_link_libraries(
		CommonLibrary
		assimp
		sfml-system
		sfml-window
		sfml-audio
		freetype
) 


