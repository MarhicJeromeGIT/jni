#pragma once


void saveTGA( char* filename );
unsigned char* readTGA(const char* filename, int& width, int& height, int& bpp, bool& hasAlpha, bool unflip = true);
