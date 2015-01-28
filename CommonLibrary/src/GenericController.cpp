#include "GenericController.h"



GenericController* GenericController::thisObject = 0;

GenericController::GenericController()
{
}


GenericController* GenericController::get()
{
	if( thisObject == 0 )
	{
		thisObject = new GenericController();
	}
	return thisObject;
}
