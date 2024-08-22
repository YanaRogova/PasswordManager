#include "EditDialog.h"
#include "ui_EditDialog.h"

EditDialog::EditDialog(QString sUsername, QString sPassword, QWidget* parent)
	: QDialog(parent)
{
	ui = new Ui::EditDialog();
	ui->setupUi(this);

	// set parameters of ui elements
	ui->lePassword->setText(sPassword);	
	ui->leUsername->setText(sUsername);	
    ui->pbOK->setFocus();

	// set icons for password visibility button
    if (this->palette().color(QPalette::Window).lightness() < 128)
    {
        m_iconHide = QIcon(":/icons/hide_light.png");
        m_iconShow = QIcon(":/icons/show_light.png");
    }
    else
    {
        m_iconHide = QIcon(":/icons/hide_dark.png");
        m_iconShow = QIcon(":/icons/show_dark.png");
    }

    OnChangePasswordVisible(); // set echo mode for the password
	
    // connect signals
	connect(ui->pbOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pbPasswordVisible, SIGNAL(clicked()), this, SLOT(OnChangePasswordVisible()));
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
	if (ui->lePassword->echoMode() == QLineEdit::Password) // show password if it was hidden
    {
        ui->pbPasswordVisible->setIcon(m_iconShow);
        ui->lePassword->setEchoMode(QLineEdit::Normal);
    }
	else // hide password if it was shown
    {
        ui->pbPasswordVisible->setIcon(m_iconHide);
        ui->lePassword->setEchoMode(QLineEdit::Password);
    }    
}