#include "EditDialog.h"
#include "ui_EditDialog.h"

EditDialog::EditDialog(QString sUsername, QString sPassword, QWidget* parent)
	: QDialog(parent)
{
	ui = new Ui::EditDialog();
	ui->setupUi(this);

	ui->lePassword->setText(sPassword);	
	ui->lePassword->setEchoMode(QLineEdit::Password);	
	ui->leUsername->setText(sUsername);	
	
	connect(ui->pbOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pbPasswordVisible, SIGNAL(clicked()), this, SLOT(OnChangePasswordVisible()));

    ui->pbOK->setFocus();
}

EditDialog::~EditDialog()
{
	delete ui;
}

QString EditDialog::GetUsername() const
{
	return ui->leUsername->text();
}

QString EditDialog::GetPassword() const
{
	return ui->lePassword->text();
}

void EditDialog::OnChangePasswordVisible()
{
    QString sPathIcon(":/icons/show_light.png");

    if (ui->lePassword->echoMode() == QLineEdit::Password)
    {
        ui->lePassword->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        sPathIcon = ":/icons/hide_light.png";
        ui->lePassword->setEchoMode(QLineEdit::Password);
    }

    ui->pbPasswordVisible->setIcon(QIcon(sPathIcon));
}