#pragma once

#include <QWidget>
#include <glm\glm.hpp>

class QImage;

class CustomQColorWidget : public QWidget
{
	Q_OBJECT

Q_SIGNALS:
	void colorChanged( const QColor& );

public:
	CustomQColorWidget( QWidget* parent );


	QImage* image;
protected:
	void paintEvent(QPaintEvent* event);
	void mousePressEvent ( QMouseEvent * e );	

};