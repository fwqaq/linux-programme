#include "widget.h"
#include <QApplication>
#include "dialolist.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Widget w;
//    w.show();
    DialoList list;
    list.show();
    return a.exec();
}
