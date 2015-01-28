#ifndef CLEANABLE_H
#define CLEANABLE_H

// delayed opengl calls (chapitre 5 "game engine gems 2")
class ICleanable
{
public:
	virtual ~ICleanable(){}
	virtual void Clean() = 0;
};

class ICleanableObserver
{
public:
	virtual ~ICleanableObserver(){}
	virtual void NotifyDirty(ICleanable *value) = 0;
};

#endif
