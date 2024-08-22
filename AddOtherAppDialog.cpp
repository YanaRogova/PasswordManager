#include "AddOtherAppDialog.h"
#include "ui_AddOtherAppDialog.h"
#include <QMessageBox>

AddOtherAppDialog::AddOtherAppDialog(QWidget* parent)
	: QDialog(parent)
{
	ui = new Ui::AddOtherAppDialog();
	ui->setupUi(this);

	connect(ui->pbOK, SIGNAL(clicked()), this, SLOT(OnOk()));
	connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

AddOtherAppDialog::~AddOtherAppDialog()
{
	delete ui;
}

void AddOtherAppDialog::OnOk()
{
	if (ui->leApp->text().isEmpty() || ui->leDescription->text().isEmpty()) 
	{
		QMessageBox::warning(this, "Warning", "Please fill in both the application name and description.");
		
		return;
	}

	accept();
}
QString AddOtherAppDialog::GetAppName() const
{
	return ui->leApp->text();
}

QString AddOtherAppDialog::GetAppDescription() const
{
	return ui->leDescription->text();
}
