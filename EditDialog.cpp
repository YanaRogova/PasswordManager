#include "EditDialog.h"
#include "ui_EditDialog.h"

EditDialog::EditDialog(QString sUsername, QString sPassword, QWidget* parent)
	: QDialog(parent)
{
	ui = new Ui::EditDialog();
	ui->setupUi(this);

	ui->lePassword->setText(sPassword);	
	ui->leUsername->setText(sUsername);	
	
	connect(ui->pbOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pbPasswordVisible, SIGNAL(clicked()), this, SLOT(OnChangePasswordVisible()));

    ui->pbOK->setFocus();

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

    OnChangePasswordVisible();
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
    if (ui->lePassword->echoMode() == QLineEdit::Password)
    {
        ui->pbPasswordVisible->setIcon(m_iconShow);
        ui->lePassword->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->pbPasswordVisible->setIcon(m_iconHide);
        ui->lePassword->setEchoMode(QLineEdit::Password);
    }    
}