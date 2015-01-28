#ifndef GENERIC_CONTROLLER_H
#define GENERIC_CONTROLLER_H

#include <stack>

struct ControllerEvent
{
	enum TYPE
	{
		ACTION_DOWN,
		ACTION_UP,
		ACTION_MOVE,
	};
	TYPE type;
	int x;
	int y;
};

class GenericController
{
private:
	static GenericController* thisObject;
	GenericController();
public:

	static GenericController* get();

public:
	std::stack<ControllerEvent> events;
};



#endif
