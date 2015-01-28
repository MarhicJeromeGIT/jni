#ifndef CUSTOM_RESOURCE_H
#define CUSTOM_RESOURCE_H

#include <string>
//#include "AntigravityConfig.h"
#include <sstream>

//Function prototypes
//int getfilesize(char *filename);
//int countfiles(char *path);
//void packfile(char *filename, int fd);
//void findfiles(char *path, int fd);

void packfile( const std::string& filepath, const std::string& name, FILE* fd);
int getfilesize(const char *filename);

void CustomResource_PackDataFiles();

std::string CustomResource_ReadFile(const char* filename);
std::wstring CustomResource_ReadFileW( const char* filename );
unsigned char* CustomResource_ReadFile(const char* filename, long& size);
FILE* CustomResource_GetFile( const char* filename );

#endif
