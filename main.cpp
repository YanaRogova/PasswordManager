#include "Manager.h"

#include <QApplication>
#include <QStyleFactory>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    QString styleSheet = 
        "QTableView::indicator:checked { image: url(:/icons/hide_light) 0; }"
        "QTableView::indicator:unchecked { image: url(:/icons/show_light.png) 0; }"
        "QCheckBox::indicator:checked { image: url(:/icons/hide_light.png) 0; }"
        "QCheckBox::indicator:unchecked { image: url(:/icons/show_light.png) 0; }";
    a.setStyleSheet(styleSheet);

    Manager w;
    w.show();
    return a.exec();
}
