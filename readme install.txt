// Needs to compile Assimp, Qt and boost

Download assimp 3.1.1 or above
http://sourceforge.net/projects/assimp/files/assimp-3.1/
and compile (cmake . -G "Visual Studio 11")

// Compiling Qt
Download and install Qt 5.0 or above
Follow http://qt-project.org/wiki/Building_Qt_5_from_Git
configure -developer-build -opensource -nomake examples -nomake tests -opengl desktop


http://stackoverflow.com/questions/15826893/building-qt5-with-visual-studio-2012-visual-studio-2013-and-integrating-with

// Compiling boost 
http://choorucode.com/2014/06/06/how-to-build-boost-for-visual-studio-2013/

