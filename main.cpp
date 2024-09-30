#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // xiugai
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
