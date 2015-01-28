#include "CustomQColorWidget.h"
#include <qpainter.h>
#include <glm\glm.hpp>
#include <qevent.h>
#include <qscrollarea.h>
#include <iostream>

using glm::vec2;
#define M_PI 3.14159265

CustomQColorWidget::CustomQColorWidget( QWidget* parent ) : QWidget( parent )
{
	int size = 200;
	resize(size, size);
	setMinimumSize( QSize( size, size ) );


	image = new QImage( size, size, QImage::Format_ARGB32 );

	uchar* data = new uchar[size*size*4];
	for( int i= 0; i < size; i++ )
	{
		for( int j= 0; j< size; j++ )
		{
			vec2 center = vec2( 0.0, 0.0 );
			float px = 2.0*((float)i) / ((float)size) -1.0;
			float py = 2.0*((float)j) / ((float)size) -1.0;
			py = -py;

			vec2 thisPoint = vec2( px, py );
			float l = glm::length( thisPoint - center );

			int a = 255;
			float r = 0;
			float g = 0;
			float b = 0;
			if( l > 0.0 )
			{
				a = l > 1.0f ? 0 : 255;

				if( a > 0 )
				{
					// what is the angle of our point :
					float sinx = py;
					float cosx = px;
					// atan2 return in [-pi;Pi]
					double angleRad = atan2(sinx,cosx);
					if( angleRad < 0 )
					{
						angleRad = M_PI + M_PI + angleRad;
					}
					
					// convert HSV to RGB
					// http://en.wikipedia.org/wiki/HSL_and_HSV#Converting_to_RGB
					float Shsv = l;
					float V    = 0.9;
					float HUE  = angleRad * 180.0f / M_PI;
					assert( HUE > -1 && HUE < 361 );
					// http://www.cs.rit.edu/~ncs/color/t_convert.html
					int Hp = HUE / 60;
					float f = HUE/60.0f - Hp;

					float p = V * (1.0f-Shsv);
					float q = V * (1.0f-Shsv * f);
					float t = V * (1.0f-Shsv*(1.0f-f));
					
					float m = 0;

					if( Hp < 1 )
					{
						r = V;
						g = t;
						b = p;
					}
					else if( Hp < 2 )
					{
						r = q;
						g = V;
						b = p;
					}
					else if( Hp < 3 )
					{
						r = p;
						g = V;
						b = t;
					}
					else if( Hp < 4 )
					{
						r = p;
						g = q;
						b = V;
					}
					else if ( Hp < 5 )
					{
						r = t;
						g = p;
						b = V;
					}
					else if( Hp < 6 )
					{
						r = V;
						g = p;
						b = q;
					}
	
					r *= 255.0f;
					g *= 255.0f;
					b *= 255.0f;
				}
			}
			
			image->setPixel(i,j, QColor(r,g,b,a).rgba()) ;

		}
	}

}

void CustomQColorWidget::paintEvent( QPaintEvent* event )
{
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
    painter.setBrush(QColor( 0,0, 0 ));

	QRect rec;
	rec.setBottomLeft( QPoint(0,0) );
	rec.setTopRight( QPoint( width(), height() ) );
	//painter.drawRect( rec );

	painter.drawImage( 0, 0, *image ); 
		
	painter.end();
}

void CustomQColorWidget::mousePressEvent ( QMouseEvent * e )
{
	QPointF p = e->localPos();

	QRgb rgb = image->pixel( QPoint( p.rx(), p.ry() ) );
	
	QColor col(rgb);
	int r = col.red();
	int g = col.green();
	int b = col.blue();

	emit colorChanged( col );

}