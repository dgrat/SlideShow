#include "app.h"
#include "image_view.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    App gui;
    gui.getGUI()->show();
    return a.exec();
}
