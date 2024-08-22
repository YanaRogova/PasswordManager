#pragma once
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
	class EditDialog;
}
QT_END_NAMESPACE

class EditDialog : public QDialog
{
	Q_OBJECT
public:
	EditDialog(QString sUsername, QString sPassword, QWidget* parent = Q_NULLPTR);
	~EditDialog();

	QString GetUsername() const;
	QString GetPassword() const;

private slots:
	void OnChangePasswordVisible();

private:
	Ui::EditDialog* ui;

	QIcon m_iconHide;
	QIcon m_iconShow;
};

