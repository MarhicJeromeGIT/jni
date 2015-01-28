
#include "mainwindow.h"

#include <qapplication.h>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    mainwindow window;
    window.resize(window.sizeHint());
    int desktopArea = QApplication::desktop()->width() *
                     QApplication::desktop()->height();
    int widgetArea = window.width() * window.height();
    if (((float)widgetArea / (float)desktopArea) < 0.75f)
        window.show();
    else
        window.showMaximized();
    return app.exec();

}
