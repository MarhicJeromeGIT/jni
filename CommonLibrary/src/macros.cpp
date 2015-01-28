#include "macros.h"

#include <stdlib.h>

float randf()
{
	return (float)rand()/(float)RAND_MAX;
}

float srandf()
{
	float r = randf();
	return (2*r)-1.0f;
}
