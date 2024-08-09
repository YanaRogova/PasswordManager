#include "AddOtherAppDialog.h"
#include "ui_AddOtherAppDialog.h"

AddOtherAppDialog::AddOtherAppDialog(QWidget* parent)
	: QDialog(parent)
{
	ui = new Ui::AddOtherAppDialog();
	ui->setupUi(this);

	connect(ui->pbOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

AddOtherAppDialog::~AddOtherAppDialog()
{
	delete ui;
}
