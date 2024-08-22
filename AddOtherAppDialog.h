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

	QString GetAppName() const;
	QString GetAppDescription() const;

private slots:
	void OnOk();

private:

private:
	Ui::AddOtherAppDialog* ui;
};

