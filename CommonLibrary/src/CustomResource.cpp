#include "CustomResource.h"
//#include "AntigravityConfig.h"
#include "assert.h"
#include <iostream>
#include <fstream>
#include "stdio.h"
//#include "dirent.h"
#include "sys/stat.h"
//#include "unistd.h"
#include "fcntl.h"
//#include "sys/param.h"
#include <stdlib.h>
#include <string.h>
#include "ShaderParams.h"

int currentfile = 1;	//This integer indicates what file we're currently adding to the resource.  The 1st?  The 5th?
int currentloc = 0;	//This integer references the current write-location within the resource file


/*
int main(int argc, char *argv[]) {
	
	char pathname[MAXPATHLEN+1];	//This character array will hold the application's working directory path
	int filecount;			//How many files are we adding to the resource?
	int fd;				//The file descriptor for the new resource
	
	//Store the current path
	getcwd(pathname, sizeof(pathname));

	//How many files are there?
	filecount = countfiles(argv[1]);
	printf("NUMBER OF FILES: %i\n", filecount);
	
	//Go back to the original path
	chdir(pathname);
	
	//How many arguments did the user pass?
	if (argc < 3)
	{
		//The user didn't specify a resource file name, go with the default
		fd = open("resource.dat", O_WRONLY | O_EXCL | O_CREAT, S_IRUSR);
	}
	else
	{
		//Use the filename specified by the user
		fd = open(argv[2], O_WRONLY | O_EXCL | O_CREAT, S_IRUSR);
	}
	//Did we get a valid file descriptor?
	if (fd < 0) 
	{
		//Can't create the file for some reason (possibly because the file already exists)
		perror("Cannot create resource file");
		exit(1);
	}
		
	//Write the total number of files as the first integer
	write(fd, &filecount, sizeof(int));
	
	//Set the current conditions
	currentfile = 1;					//Start of by storing the first file, obviously!
	currentloc = (sizeof(int) * filecount) + sizeof(int);	//Leave space at the begining of the resource file for the header info
	
	//Use the findfiles routine to pack in all the files
	findfiles(argv[1], fd);
	
	//Close the file
	close(fd);
	
	return 0;
}

*/

/*

// *****************************************************************
//
// This routine recurses through all subdirectories of a given path,
// and counts all of the files it encounters.
//
// *****************************************************************
int countfiles(char *path) {
	
	int count = 0;			//This integer will count up all the files we encounter
	struct dirent *entry;		//This structure will hold file information
	struct stat file_status;	//This structure will be used to query file status
	DIR *dir = opendir(path);	//This pointer references the directory stream
		
	//Make sure we have a directory stream pointer
	if (!dir) {
		perror("opendir failure");
		exit(1);
	}
	
	//Change directory to the given path
	chdir(path);
	
	//Loop through all files and directories
	while ( (entry = readdir(dir)) != NULL) {
		//Don't bother with the .. and . directories
		if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
			//Get the status info for the current file
			if (stat(entry->d_name, &file_status) == 0) {
				//Is this a directory, or a file?
				if (S_ISDIR(file_status.st_mode)) {
					//Call countfiles again (recursion) and add the result to the count total
					count += countfiles(entry->d_name);
					chdir("..");
				}
				else {
					//We've found a file, increment the count
					count++;
				}
			}
		}
	}
	
	//Make sure we close the directory stream
	if (closedir(dir) == -1) {
		perror("closedir failure");
		exit(1);
	}
	
	//Return the file count
	return count;	
}

*/

/*
// *****************************************************************
//
// This routine recurses through all subdirectories of a given path,
// and calls the packfile function on any files it encounters.
//
// *****************************************************************
void findfiles(char *path, int fd) {
	
	struct dirent *entry;		//This structure will hold file information
	struct stat file_status;	//This structure will be used to query file status
	DIR *dir = opendir(path);	//This pointer references the directory stream
		
	//Make sure we have a directory stream pointer
	if (!dir) {
		perror("opendir failure");
		exit(1);
	}
	
	//Change directory to the given path
	chdir(path);
	
	//Loop through all files and directories
	while ( (entry = readdir(dir)) != NULL) {
		//Don't bother with the .. and . directories
		if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
			//Get the status info for the current file
			if (stat(entry->d_name, &file_status) == 0) {
				//Is this a directory, or a file?
				if (S_ISDIR(file_status.st_mode)) {
					//Call findfiles again (recursion), passing the new directory's path
					findfiles(entry->d_name, fd);
					chdir("..");
				}
				else {
					//We've found a file, pack it into the resource file
					packfile(entry->d_name, fd);
				}
			}
		}
	}
	
	//Make sure we close the directory stream
	if (closedir(dir) == -1) {
		perror("closedir failure");
		exit(1);
	}
	
	return;	
}

*/
// *****************************************************************
//
// This routine accepts a file path and a file descriptor as 
// parameters.  The file specified by the path is opened, and its
// contents are written to the file specified by the descriptor.
//
// *****************************************************************
void packfile( const std::string& filepath, const std::string& name, FILE* fd)
{
	std::string filename = filepath + name;
	
	int totalsize = 0;	//This integer will be used to track the total number of bytes written to file
	
	//Handy little output
	printf("PACKING: '%s' SIZE: %i\n", filename.c_str(), getfilesize(filename.c_str()));
	
	//In the 'header' area of the resource, write the location of the file about to be added
	fseek(fd, currentfile * sizeof(int), SEEK_SET);
	fwrite( &currentloc, sizeof(int), 1, fd);
	
	//Seek to the location where we'll be storing this new file info
	fseek(fd, currentloc, SEEK_SET);
	
	//Write the size of the file
	int filesize = getfilesize(filename.c_str());
	fwrite( &filesize, sizeof(filesize), 1, fd );
	totalsize += sizeof(int);
	
	//Write the LENGTH of the NAME of the file
	int filenamelen = strlen(filename.c_str());
	fwrite(&filenamelen, sizeof(int),1 , fd );
	totalsize += sizeof(int);
	
	//Write the name of the file
	fwrite( filename.c_str(), strlen(filename.c_str()), 1, fd);
	totalsize += strlen(filename.c_str());
	
	//Write the file contents
	FILE* fd_read = fopen(filename.c_str(), "rb");		//Open the file
	assert( fd_read != NULL );
	char *buffer = new char[filesize];	//Create a buffer for its contents
	fread( buffer, sizeof(char), filesize, fd_read );		//Read the contents into the buffer
	fwrite(buffer, sizeof(char), filesize, fd );			//Write the buffer to the resource file
	fclose(fd_read);					//Close the file
	delete[] buffer;					//Free the buffer
	totalsize += filesize;				//Add the file size to the total number of bytes written
	
	//Increment the currentloc and current file values
	currentfile++;
	currentloc += totalsize;
}

// *****************************************************************
//
// This routine determines the size of a file specified by a given 
// path.
//
// *****************************************************************
int getfilesize(const char *filename) {
	
	struct stat file;	//This structure will be used to query file status
	
	//Extract the file status info
	if(!stat(filename, &file))
	{
		//Return the file size
		return file.st_size;
	}
	   
	//ERROR! Couldn't get the filesize.
	printf("getfilesize:  Couldn't get filesize of '%s'.", filename);
	exit(1);
}

void CustomResource_PackDataFiles()
{
	/*
	std::string path = ShaderParams::get()->currentDir + std::string( "gamedata.dat" );
	FILE* fd = fopen( path.c_str(), "wb" );

	int filecount = 10; // safety
	filecount += 12;
	filecount += 11;
	filecount += 13;
	filecount += 12;
	filecount += 26;
	filecount += 25;
	filecount += 6;
	filecount += 5;

	//Write the total number of files as the first integer
	fwrite( &filecount, sizeof(int), 1, fd );

	//Set the current conditions
	currentfile = 1;					//Start of by storing the first file, obviously!
	currentloc = (sizeof(int) * filecount) + sizeof(int);	//Leave space at the begining of the resource file for the header info

	// 11
	packfile( DATA_PATH, "titlescreen.png", fd );
	packfile( DATA_PATH, "Powerup.png", fd );
	packfile( DATA_PATH, "alien_diffuse_2.png", fd );
	packfile( DATA_PATH, "bubble_big.png", fd );
	packfile( DATA_PATH, "spaceship_normal.tga", fd );
	packfile( DATA_PATH, "spaceship_diffuse.png", fd );
	packfile( DATA_PATH, "alien_diffuse.png", fd );
	packfile( DATA_PATH, "alien_normal.png", fd );
	packfile( DATA_PATH, "bubble.png", fd );
	packfile( DATA_PATH, "space_cartoon.png", fd );
	packfile( DATA_PATH, "icon.png", fd );
	packfile( DATA_PATH, "bandanatech.png", fd );

	// 11
	packfile( DATA_PATH, "grapin.png", fd );
	packfile( DATA_PATH, "levelselect.png", fd );
	packfile( DATA_PATH, "endlevel.png", fd );
	packfile( DATA_PATH, "spacebox_right.png", fd );
	packfile( DATA_PATH, "spacebox_left.png", fd );
	packfile( DATA_PATH, "spacebox_front.png", fd );
	packfile( DATA_PATH, "spacebox_back.png", fd );
	packfile( DATA_PATH, "spacebox_top.png", fd );
	packfile( DATA_PATH, "spacebox_bottom.png", fd );
	packfile( DATA_PATH, "victory.png", fd );
	packfile( DATA_PATH, "slick_arrow-arrow.png", fd );

	// 13
	packfile( DATA_PATH, "lines2.png", fd );
	packfile( DATA_PATH, "dpadinner.png", fd );
	packfile( DATA_PATH, "dpadouter.png", fd );
	packfile( DATA_PATH, "harpoon.png", fd );
	packfile( DATA_PATH, "Metal_Cover_1k_d.tga", fd );
	packfile( DATA_PATH, "metal1-dif-1024.tga", fd );
	packfile( DATA_PATH, "metal2-dif-1024.tga", fd );
	packfile( DATA_PATH, "numbers.png", fd );
	packfile( DATA_PATH, "icons.png", fd );
	packfile( DATA_PATH, "earthmap.tga", fd );
	packfile( DATA_PATH, "spaceship_diffuse.tga", fd );
	packfile( DATA_PATH, "alien_freestyle.png", fd );
	packfile( DATA_PATH, "slider.tga", fd );

	// 12
	packfile( DATA_PATH, "player.txt", fd );
	packfile( DATA_PATH, "skybox.txt", fd );
	packfile( DATA_PATH, "spaceship.txt", fd );
	packfile( DATA_PATH, "ballBig.txt", fd );
	packfile( DATA_PATH, "walls.txt", fd );
	packfile( DATA_PATH, "ladder.txt", fd );
	packfile( DATA_PATH, "torus.txt", fd );
	packfile( DATA_PATH, "earth.txt", fd );
	packfile( DATA_PATH, "disquemodel.txt", fd );
	packfile( DATA_PATH, "alien_title.txt", fd );
	packfile( DATA_PATH, "bouncingball.txt", fd );
	packfile( DATA_PATH, "credits.txt", fd );

	// 26
	packfile( shader_path, "BumpShader.vs", fd );
	packfile( shader_path, "CelShader.vs", fd );
	packfile( shader_path, "CubemapReflectionShader.vs", fd );
	packfile( shader_path, "DiffuseShader.vs", fd );
	packfile( shader_path, "dummy_shader.vs", fd );
	packfile( shader_path, "ExplosionWaveShader.vs", fd );
	packfile( shader_path, "GaussianBlurShader.vs", fd );
	packfile( shader_path, "LaserShader.vs", fd );
	packfile( shader_path, "LineShader.vs", fd );
	packfile( shader_path, "MorphShader.vs", fd );
	packfile( shader_path, "OceanShader.vs", fd );
	packfile( shader_path, "ParticleShader.vs", fd );
	packfile( shader_path, "PhongShader.vs", fd );
	packfile( shader_path, "PhongTextureShader.vs", fd );
	packfile( shader_path, "RefractShader.vs", fd );
	packfile( shader_path, "shadowMapGenerator.vs", fd );
	packfile( shader_path, "shadowMapSkinningGenerator.vs", fd );
	packfile( shader_path, "ShockwaveShader.vs", fd );
	packfile( shader_path, "SimpleTextureShader.vs", fd );
	packfile( shader_path, "skinningshader.vs", fd );
	packfile( shader_path, "SkinningSpecularShader.vs", fd );
	packfile( shader_path, "skyboxShader.vs", fd );
	packfile( shader_path, "SobelShader.vs", fd );
	packfile( shader_path, "TextShader.vs", fd );
	packfile( shader_path, "TextureMixShader.vs", fd );
	packfile( shader_path, "TextureShader.vs", fd );

	// 25
	packfile( shader_path, "BumpShader.ps", fd );
	packfile( shader_path, "CelShader.ps", fd );
	packfile( shader_path, "CubemapReflectionShader.ps", fd );
	packfile( shader_path, "DiffuseShader.ps", fd );
	packfile( shader_path, "dummy_shader.ps", fd );
	packfile( shader_path, "ExplosionWaveShader.ps", fd );
	packfile( shader_path, "GaussianBlurShader.ps", fd );
	packfile( shader_path, "LaserShader.ps", fd );
	packfile( shader_path, "LineShader.ps", fd );
	packfile( shader_path, "MorphShader.ps", fd );
	packfile( shader_path, "OceanShader.ps", fd );
	packfile( shader_path, "ParticleShader.ps", fd );
	packfile( shader_path, "PhongShader.ps", fd );
	packfile( shader_path, "PhongTextureShader.ps", fd );
	packfile( shader_path, "RefractShader.ps", fd );
	packfile( shader_path, "shadowMapGenerator.ps", fd );
	packfile( shader_path, "ShockwaveShader.ps", fd );
	packfile( shader_path, "SimpleTextureShader.ps", fd );
	packfile( shader_path, "skinningshader.ps", fd );
	packfile( shader_path, "SkinningSpecularShader.ps", fd );
	packfile( shader_path, "skyboxShader.ps", fd );
	packfile( shader_path, "SobelShader.ps", fd );
	packfile( shader_path, "TextShader.ps", fd );
	packfile( shader_path, "TextureMixShader.ps", fd );
	packfile( shader_path, "TextureShader.ps", fd );

	// 6
	packfile( DATA_PATH, "Sisters of Snow Assent Dissent.ogg", fd );
	packfile( DATA_PATH, "84327__splashdust__sadwhisle.wav", fd );
	packfile( DATA_PATH, "140867__juskiddink__boing.wav", fd );
	packfile( DATA_PATH, "202230__deraj__pop-sound.wav", fd );
	packfile( DATA_PATH, "Push Clicky Buttons005.wav", fd );
	packfile( DATA_PATH, "191754__fins__button-5.wav", fd );


	// fonts
	packfile( DATA_PATH, "DroidSans.ttf", fd );
	packfile( DATA_PATH, "DroidSans-Bold.ttf", fd );
	packfile( DATA_PATH, "DroidSerif-BoldItalic.ttf", fd );
	packfile( DATA_PATH, "cinnamon cake.ttf", fd );
	packfile( DATA_PATH, "NanumMyeongjo.ttf", fd );


	fclose(fd);*/
}


std::string CustomResource_ReadFile(const char* filename)
{
	std::string path = ShaderParams::get()->currentDir + std::string( "gamedata.dat" );
	FILE* fd = fopen( path.c_str(), "rb" );
	if( fd == NULL )
	{
		return  "";
	}

	std::string content = "";

	// find the file :
	int nbFiles = 0;
	fread( &nbFiles, sizeof(int), 1, fd );
	for( int i=0; i< nbFiles; i++ )
	{
		int currentLoc = 0;
		fread( &currentLoc, sizeof(int), 1, fd );

		fseek(fd, currentLoc, SEEK_SET);
		// find the file name :
		int filesize, namelength;
		fread( &filesize, sizeof(int), 1, fd );
		fread( &namelength, sizeof(int), 1, fd );

		char* name = new char[namelength+1];
		fread( name, sizeof(char), namelength, fd );
		name[namelength] = 0;

		if( strcmp( name, filename ) == 0 )
		{
			std::cout<<" FOUND IT "<<std::endl;

			char* buffer = new char[filesize+1];
			fread( buffer, sizeof(char), filesize, fd );
			buffer[filesize] = 0;

			content += buffer;
			delete[] buffer;
			delete[] name;
			break;
		}

		delete[] name;

		fseek( fd, (1 + i) * sizeof(int), SEEK_SET );
	}

	fclose(fd);
	return content;
}

unsigned char* CustomResource_ReadFile(const char* filename, long& size)
{ 
	std::string path = ShaderParams::get()->currentDir + std::string( "gamedata.dat" );
	FILE* fd = fopen( path.c_str(), "rb" );
	if( fd == NULL )
	{
		return NULL; 
	}

	unsigned char* content = NULL;
	size = 0;

	// find the file :
	int nbFiles = 0;
	fread( &nbFiles, sizeof(int), 1, fd );
	for( int i=0; i< nbFiles; i++ )
	{
		int currentLoc = 0;
		fread( &currentLoc, sizeof(int), 1, fd );

		fseek(fd, currentLoc, SEEK_SET);
		// find the file name :
		int filesize, namelength;
		fread( &filesize, sizeof(int), 1, fd );
		fread( &namelength, sizeof(int), 1, fd );

		char* name = new char[namelength+1];
		fread( name, sizeof(char), namelength, fd );
		name[namelength] = 0;

		if( strcmp( name, filename ) == 0 )
		{
			std::cout<<" FOUND IT "<<std::endl;

			size = filesize;
			content = new unsigned char[filesize+1];
			fread( content, sizeof(char), filesize, fd );
			content[filesize] = 0;

			delete[] name;
			break;
		}

		delete[] name;

		fseek( fd, (1 + i) * sizeof(int), SEEK_SET );
	}

	fclose(fd);

	return content;
}

std::wstring CustomResource_ReadFileW( const char* filename )
{	
	// not implemented yet:
	std::string str = CustomResource_ReadFile( filename );
	std::wstring wstr( str.begin(), str.end() );
	return wstr;
}

FILE* CustomResource_GetFile( const char* filename )
{
	std::string path = ShaderParams::get()->currentDir + std::string( "gamedata.dat" );
	FILE* fd = fopen( path.c_str(), "rb" );
	if( fd == NULL )
	{
		return  NULL;
	}

	// find the file :
	bool found = false;
	int nbFiles = 0;
	fread( &nbFiles, sizeof(int), 1, fd );
	for( int i=0; i< nbFiles; i++ )
	{
		int currentLoc = 0;
		fread( &currentLoc, sizeof(int), 1, fd );

		fseek(fd, currentLoc, SEEK_SET);
		// find the file name :
		int filesize, namelength;
		fread( &filesize, sizeof(int), 1, fd );
		fread( &namelength, sizeof(int), 1, fd );

		char* name = new char[namelength+1];
		fread( name, sizeof(char), namelength, fd );
		name[namelength] = 0;

		if( strcmp( name, filename ) == 0 )
		{
			std::cout<<" FOUND IT "<<std::endl;
			delete[] name;
			found = true;
			break;
		}

		fseek( fd, (1 + i) * sizeof(int), SEEK_SET );
	}
	assert( found );
	return fd;
}
