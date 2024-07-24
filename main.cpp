#include "Manager.h"

#include <QApplication>
#include <QStyleFactory>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    Manager w;
    w.show();
    return a.exec();
}
