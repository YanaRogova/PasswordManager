#pragma once
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
	class AddOtherAppDialog;
}
QT_END_NAMESPACE

class AddOtherAppDialog : public QDialog
{
	Q_OBJECT
public:
	AddOtherAppDialog(QWidget* parent = Q_NULLPTR);
	~AddOtherAppDialog();

private:
	Ui::AddOtherAppDialog* ui;
};

