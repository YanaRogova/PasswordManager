#include "Manager.h"
#include "ui_Manager.h"
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include "EditDialog.h"
#include "AddOtherAppDialog.h"

Manager::Manager(QWidget *parent)
    : QMainWindow(parent), 
    ui(new Ui::Manager), 
    m_saverLoader(QCoreApplication::applicationDirPath()),
    m_bChanges(false)
{
    // Create central widget with main layout
    QWidget *centralWidget = new QWidget(this);
    ui->setupUi(centralWidget);
    setCentralWidget(centralWidget);

    //set icons for light or dark backgrounds
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

    SetManagerUi();
    CreateCustomMenu();
    LoadAccounts();

    // connect signals
    connect(ui->cbApp, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeCurrentApp()));
    connect(ui->pbAddAccount, SIGNAL(clicked()), this, SLOT(OnAddAccaunt()));
    connect(ui->pbAddApp, SIGNAL(clicked()), this, SLOT(OnAddApp()));
    connect(ui->tblAccounts, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(OnChangeTableItemVisible(QTableWidgetItem*)));
    connect(ui->pbPasswordVisible, SIGNAL(clicked()), this, SLOT(OnChangePasswordVisible()));
    connect(ui->pbDeleteApp, SIGNAL(clicked()), this, SLOT(OnRemoveApp()));
    connect(ui->rbDeviceApps, SIGNAL(clicked()), this, SLOT(OnChangeGroup()));
    connect(ui->rbOtherApps, SIGNAL(clicked()), this, SLOT(OnChangeGroup()));
    connect(ui->tblAccounts, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnShowMenu(QPoint)));
}

Manager::~Manager()
{
    delete ui;
}

void Manager::SetManagerUi()
{
    // set main window
    setWindowFlags(Qt::WindowCloseButtonHint);
    setWindowTitle("Password manager");

    ui->rbDeviceApps->setChecked(true);

    // set combobox with application names
    ui->cbApp->setEnabled(true);
    ui->cbApp->setInsertPolicy(QComboBox::InsertPolicy::InsertAlphabetically);

	// set line edits for account username and password
    ui->leUser->setPlaceholderText("Username");
    ui->lePassword->setPlaceholderText("Password");
    ui->lePassword->setEchoMode(QLineEdit::Password);
    ui->pbPasswordVisible->setIcon(m_iconHide);

    ui->rbDeviceApps->setChecked(true);

    ui->tblAccounts->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    DisableAccountAdding(true);

    this->setWindowIcon(QIcon(":/icons/manager.png"));
}

void Manager::CreateCustomMenu()
{
    m_customMenu = new QMenu(this);

	// create actions for custom menu
    QAction* copyAccount = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy), "Copy", this);
    QAction* editAccount = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::MailMessageNew), "Edit", this);
    QAction* removeAccount = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete), "Remove", this);

	// connect actions to slots
    connect(copyAccount, SIGNAL(triggered()), this, SLOT(OnCopyAccount()));
    connect(editAccount, SIGNAL(triggered()), this, SLOT(OnEditAccount()));
    connect(removeAccount, SIGNAL(triggered()), this, SLOT(OnRemoveAccount()));

	// add actions to custom menu
    m_customMenu->addAction(copyAccount);
    m_customMenu->addAction(editAccount);
    m_customMenu->addAction(removeAccount);
}

void Manager::DisableAccountAdding(bool bDisable)
{
	// disable/enable account adding buttons and line edits
    ui->pbAddAccount->setDisabled(bDisable);
    ui->leUser->setDisabled(bDisable);
    ui->lePassword->setDisabled(bDisable);
    ui->pbPasswordVisible->setDisabled(bDisable);
    ui->pbDeleteApp->setDisabled(bDisable);
}

void Manager::LoadAccounts()
{
	// load accounts from files and set combobox items
    m_saverLoader.LoadAccounts(m_deviceApps, m_otherApps);

    SetComboBoxItems(m_deviceApps);
    ui->cbApp->setCurrentIndex(-1);
}

void Manager::SetComboBoxItems(const AppsManager& appsManager)
{
	ui->cbApp->clear();

	// add application names and descriptions to combobox items
    for (auto sAppName : appsManager.GetAppNames())
    {
        ui->cbApp->insertItem(0, sAppName, appsManager.GetAppDescription(sAppName));
        ui->cbApp->setItemData(0, appsManager.GetAppDescription(sAppName), Qt::ToolTipRole);
    }
       
	ui->cbApp->setCurrentIndex(0); // select first item by current
}

void Manager::AddAppFromDevice()
{
	// open file dialog to select application executable file
    QString sFilter("Executable files (*.exe)");
    QFileDialog fdlg(this, "Select application executable file", "/C:/", sFilter);
	fdlg.setFileMode(QFileDialog::ExistingFile); // select only single existing file

    if(!fdlg.exec())
        return;

	QStringList fileNames = fdlg.selectedFiles(); // get selected file path

    for (auto filePath : fileNames)
    {
        QFileInfo fInfo(filePath);
        QString fName(fInfo.baseName());

		if (!m_deviceApps.AddApp(fName, filePath)) // app has already been added
        {
            QMessageBox::information(this, "The application has already been added",
                "The selected application has already been added. You can add a new account for this application.");

			ui->cbApp->setCurrentIndex(ui->cbApp->findText(fName)); // select added app in combobox
            return;
        }
    }

	// update combobox items
    SetComboBoxItems(m_deviceApps);
    m_bChanges = true;
}

void Manager::AddOtherApp()
{
	AddOtherAppDialog dlg(this); // create dialog with input fields for app name and description

	if (dlg.exec() == QDialog::Accepted) // user clicked OK
    {
		QString sAppName = dlg.GetAppName();
		QString sAppDescription = dlg.GetAppDescription();

		if (!m_otherApps.AddApp(sAppName, sAppDescription)) // app has already been added
        {
            QMessageBox::information(this, "The application has already been added",
                "The selected application has already been added. You can add a new account for this application.");

            ui->cbApp->setCurrentIndex(ui->cbApp->findText(sAppName));
            return;
        }

		// update combobox items
        SetComboBoxItems(m_otherApps);
        m_bChanges = true;        
    }
}

void Manager::UpdateAccountsTable(QString sAppName)
{
	// clear table and update with accounts for application with the given name from selected group
    UpdateAccountsTableForGroup(sAppName, ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::UpdateAccountsTableForGroup(const QString& sAppName, const AppsManager& appsManager)
{
	// get accounts for the app with the given name
	QMap<QString, QString> appAccounts = appsManager.GetAppAccounts(sAppName);

    ui->tblAccounts->clearContents();
    ui->tblAccounts->setRowCount(appAccounts.size());

    // set table items
    int nRowCount = 0;
    for (auto sUser : appAccounts.keys())
    {
        // create table item with username
        QTableWidgetItem* twiUser = new QTableWidgetItem(sUser);
        ui->tblAccounts->setItem(nRowCount, COL_USER, twiUser);

		// create table item with password and hide password
        QTableWidgetItem* twiPassword = new QTableWidgetItem(QString::QString(appAccounts.value(sUser).size(), '*'));
        twiPassword->setData(Qt::UserRole, appAccounts.value(sUser));
        twiPassword->setCheckState(Qt::Checked);
        ui->tblAccounts->setItem(nRowCount, COL_PASSWORD, twiPassword);

        nRowCount++;
    }
}

void Manager::closeEvent(QCloseEvent* event)
{
	if (!m_bChanges) // no changes made
    {
        event->accept(); 
        return;
    }

    QMessageBox msbSave(QMessageBox::Question,
        "Save changes",
        "Do you want to save changes?",
        QMessageBox::Yes | QMessageBox::No);

    int res = msbSave.exec();
	if (res == QMessageBox::No) // don't save changes
    {
        event->accept();
        return;
    }
	else if (res == QMessageBox::Yes) // save changes
    {
		// save changes to files
        bool bSaved = m_saverLoader.SaveAccounts(m_deviceApps, m_otherApps);

		if (!bSaved) // failed to save changes
        {
            QMessageBox msb(QMessageBox::Question,
                "Failed to save changes",
                "Failed to save changes. Close the app anyway?",
                QMessageBox::Yes | QMessageBox::No);

            if (msb.exec() != QMessageBox::Yes)
            {
                event->ignore();
                return;
            }
        }

        event->accept();
        return;
    }
	else // don't close the app
    {
        event->ignore();
        return;
    }
}

void Manager::AddAccountForGroup(AppsManager& appsManager)
{
	// check if all fields are filled in
    if (ui->leUser->text().isEmpty() || ui->lePassword->text().isEmpty())
    {
        QMessageBox::information(this, "Some fields are not filled in",
            "The username or password isn't filled in. Fill in the missing fields.", QMessageBox::Ok);
        return;
    }

    QString sAppName = ui->cbApp->currentText();

	//add account for the current application in the selected group and update accounts table
    if (appsManager.AddAppAccount(sAppName, ui->leUser->text(), ui->lePassword->text()))
	{
		UpdateAccountsTable(sAppName);

		ui->leUser->clear();
		ui->lePassword->clear();
		ui->pbPasswordVisible->setChecked(false);
		m_bChanges = true;
	}    
}

void Manager::RemoveAppFromGroup(AppsManager& appsManager)
{
    int indCurrentApp = ui->cbApp->currentIndex();

	if (indCurrentApp != -1) // application selected
    {
        QMessageBox msb(QMessageBox::Question,
            "Remove application",
            "Are you sure you want to remove the application from the list and all accounts added to it?",
            QMessageBox::Yes | QMessageBox::No);

		if (msb.exec() != QMessageBox::Yes) // don't remove application
        {
            return;
        }

		// remove application from selected group
        QString sAppName = ui->cbApp->currentText();
		appsManager.RemoveApp(sAppName);

		// remove application from combobox items if it exists
        ui->cbApp->removeItem(indCurrentApp);
        ui->cbApp->setCurrentIndex(-1);

        UpdateAccountsTable();
        m_bChanges = true;
    }
}

void Manager::RemoveAccountFromGroup(AppsManager& appsManager)
{
	if (!ui->tblAccounts->currentItem()) // no account selected
        return;

    QMessageBox msb(QMessageBox::Question,
        "Remove account",
        "Are you sure you want to remove the current account?",
        QMessageBox::Yes | QMessageBox::No);

	if (msb.exec() != QMessageBox::Yes) // don't remove account
    {
        return;
    }

	// remove account from the current application in the selected group
    QString sUserName(ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_USER)->text());
	appsManager.RemoveAppAccount(ui->cbApp->currentText(), sUserName);
    
	// remove account from table
    ui->tblAccounts->removeRow(ui->tblAccounts->currentRow());
    m_bChanges = true;
}

void Manager::EditAccountInGroup(AppsManager& appsManager)
{
	if (!ui->tblAccounts->currentItem()) // no account selected
        return;

	// create dialog to edit account data
    EditDialog dlg(ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_USER)->text(),
        ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_PASSWORD)->data(Qt::UserRole).toString(), this);

	if (dlg.exec() == QDialog::Accepted) // user clicked OK
    {
		//get table items of the current account
        QTableWidgetItem* currUsername = ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_USER);
        QTableWidgetItem* currPassword = ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_PASSWORD);

		// get new username and password from dialog
        QString sNewUsername = dlg.GetUsername();
        QString sNewPassword = dlg.GetPassword();

        if (!currUsername || !currPassword ||
            (currUsername->text() == sNewUsername &&
			currPassword->data(Qt::UserRole).toString() == sNewPassword)) // no changes made
            return;

		// remove old account from the application in the selected group and add new account
        QString sAppName(ui->cbApp->currentText());
		appsManager.RemoveAppAccount(sAppName, currUsername->text());
		appsManager.AddAppAccount(sAppName, sNewUsername, sNewPassword);

		// update table items with new account data
        currUsername->setText(sNewUsername);
        currPassword->setText(sNewPassword);
        currPassword->setData(Qt::UserRole, sNewPassword);
        OnChangeTableItemVisible(currPassword);

        m_bChanges = true;
    }
}

void Manager::OnChangeCurrentApp()
{
	if (ui->cbApp->currentIndex() == -1) // no application selected
    {
		DisableAccountAdding(true); // disable account adding
        return;
    }

	DisableAccountAdding(false); // enable account adding

	// update accounts table with accounts for the current application from selected group
    QString sCurrentAppName = ui->cbApp->currentText();
    UpdateAccountsTable(sCurrentAppName);
}

void Manager::OnAddAccaunt()
{
	// add account for the current application in the current group
    AddAccountForGroup(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::OnAddApp()
{
	if (ui->rbDeviceApps->isChecked()) // add application to the group of application from this device
    {
        AddAppFromDevice();
    }
    else if (ui->rbOtherApps->isChecked()) // add application to the group of application not from this device
    {
        AddOtherApp();
    }

	DisableAccountAdding(ui->cbApp->currentIndex() == -1); // disable account adding if no application selected
}

void Manager::OnRemoveApp()
{
	// remove application from the current group
    RemoveAppFromGroup(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::OnChangeTableItemVisible(QTableWidgetItem* ptwi)
{
	if (ptwi && ptwi->column() == COL_PASSWORD) // password column
    {
		if (ptwi->checkState() == Qt::Checked) // password is hidden
        {
            ptwi->setText(QString::QString(ptwi->text().size(), '*'));
        }
		else // password is shown
        {
            ptwi->setText(ptwi->data(Qt::UserRole).toString());
        }
    }
}

void Manager::OnChangePasswordVisible()
{
	if (ui->lePassword->echoMode() == QLineEdit::Password) // password was hidden, show it
    {
        ui->pbPasswordVisible->setIcon(m_iconShow);
        ui->lePassword->setEchoMode(QLineEdit::Normal);
    }
	else // password was shown, hide it
    {
        ui->pbPasswordVisible->setIcon(m_iconHide);
        ui->lePassword->setEchoMode(QLineEdit::Password);
    }
}

void Manager::OnShowMenu(QPoint pos)
{
	if (!ui->tblAccounts->itemAt(pos)) // no account on this position
        return;

	// create context menu for account
    m_customMenu->popup(ui->tblAccounts->viewport()->mapToGlobal(pos));
}

void Manager::OnEditAccount()
{
	// edit account of the current application in the current group of applications
    EditAccountInGroup(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::OnRemoveAccount()
{
	// remove account of the current application from the current group of applications
    RemoveAccountFromGroup(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::OnCopyAccount()
{
	QTableWidgetItem* currItem = ui->tblAccounts->currentItem(); 
    if (!currItem) // no account selected
        return;

    QClipboard* clipboard = QApplication::clipboard();

    if (currItem->column() == COL_USER)
		clipboard->setText(currItem->text()); // copy username
    else if (currItem->column() == COL_PASSWORD)
		clipboard->setText(currItem->data(Qt::UserRole).toString()); // copy password
}

void Manager::OnChangeGroup()
{
	// switch between device applications and other applications group
    SetComboBoxItems(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
	ui->cbApp->setCurrentIndex(-1);
    UpdateAccountsTable();
}

