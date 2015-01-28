#ifndef QTWEAKABLE_H
#define QTWEAKABLE_H

#include <string>
#include <iostream>
#include <vector>
#include <thread>

#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <boost/function.hpp>

#include <QtCore/QCoreApplication>
#include <qwidget.h>

enum TWEAKER_TYPE
{
	TWEAKER_UNKNOW, // unsupported type
	TWEAKER_FLOAT,
	TWEAKER_INTEGER,
	TWEAKER_COLOR,
	TWEAKER_BOOL,
	TWEAKER_BUTTON,
	TWEAKER_ENUM,
	TWEAKER_FILECHOOSER,
	TWEAKER_FILESAVER,

	TWEAKER_TYPE_MAX,
};

//*****************************************************
// Tweakable base-class (abstract)
//
//*****************************************************
typedef boost::function<void(float)>       callbackFloatArg;
typedef boost::function<void(glm::vec4)>        callbackFloatVec4;
typedef boost::function<void(void)>        callbackVoid;
typedef boost::function<void(bool)>        callbackBoolArg;
typedef boost::function<void(int)>         callbackIntArg;
typedef boost::function<void(std::string)> callbackStringArg;

class QTweakable : public QObject
{
	Q_OBJECT

public:
	int TWEAK_ID;
	TWEAKER_TYPE type;
	std::string name;

public:
	QTweakable();
	virtual ~QTweakable(){}	
};


//*****************************************************
// Boolean Tweakable
//
//*****************************************************

class QBooleanTweakable : public QTweakable
{
	Q_OBJECT
public:
	bool bValue;
	callbackBoolArg callback;
	QBooleanTweakable( const std::string& n, callbackBoolArg cb, bool val = false );

public slots:
	void doCallback( int b );
};

//*****************************************************
// Float Tweakable
//
//*****************************************************

class QFloatTweakable : public QTweakable
{
	Q_OBJECT
public:

	float fValue;
	float fMin;
	float fMax;
	callbackFloatArg callback;

	QFloatTweakable( const std::string& n, callbackFloatArg cb, float val = 0.0, float min=0.0, float max=0.0 );

public slots:
	void doCallback( double f );
};

//*****************************************************
// Int Tweakable
//
//*****************************************************

struct QIntTweakable : public QTweakable
{
	Q_OBJECT

public slots:
	void doCallback(int value);

public:
	int iValue;
	int iMin;
	int iMax;
	callbackIntArg callback;

	QIntTweakable( const std::string& n, callbackFloatArg cb, int val = 0, int min =0, int max = 100 );
};

//*****************************************************
// Color Tweakable
//
//*****************************************************


class QColorTweakable : public QTweakable
{
	Q_OBJECT

public slots:
	void doCallback( const QColor & color);

public:
	glm::vec4 color;
	callbackFloatVec4 callback;

	QColorTweakable( const std::string& n, callbackFloatVec4 cb, const glm::vec4& val );


};

//*****************************************************
// A Button... yep.
//
//*****************************************************

struct QButtonTweakable : public QTweakable
{
	callbackVoid callback;

	QButtonTweakable( const std::string& n, callbackVoid cb );
};

//*****************************************************
// A choice selection tweakable
//
//*****************************************************

struct QEnumTweakable : public QTweakable
{
	callbackIntArg callback;
	std::vector<std::string> strings;
	int currentChoice;

	QEnumTweakable( const std::string& n, callbackIntArg cb, std::vector<std::string> strings );
	QEnumTweakable( const std::string& n, callbackIntArg cb, int nbStrings, const std::string* strings );

};


#endif
