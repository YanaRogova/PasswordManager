#include "Manager.h"

#include <QApplication>
#include <QStyleFactory>
#include <QPalette>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	auto style = QStyleFactory::create("Fusion");
    a.setStyle(style);

    QString styleSheet;
	QColor bgColor = a.palette().color(QPalette::Window);
    if (bgColor.lightness() < 128)
    {
        styleSheet = 
            "QTableView::indicator:checked { image: url(:/icons/hide_light.png) 0; }"
            "QTableView::indicator:unchecked { image: url(:/icons/show_light.png) 0; }";
    }
    else
    {
		styleSheet = 
            "QTableView::indicator:checked { image: url(:/icons/hide_dark.png) 0; }"
            "QTableView::indicator:unchecked { image: url(:/icons/show_dark.png) 0; }";
    }
    
    a.setStyleSheet(styleSheet);

    Manager w;
    w.show();
    return a.exec();
}
