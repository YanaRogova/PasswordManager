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

   // m_leOtherAppName = new QLineEdit(centralWidget);
   //auto grid = qobject_cast<QGridLayout*>(centralWidget->layout());
   // grid->addWidget(m_leOtherAppName, 4, 0, 1, 5);
   // m_leOtherAppName->hide();

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

    // set font
    ui->leUser->setPlaceholderText("Username");
    ui->lePassword->setPlaceholderText("Password");
    ui->lePassword->setEchoMode(QLineEdit::Password);
    ui->pbPasswordVisible->setIcon(QIcon(":/icons/hide_light.png"));
    // QFont font = ui->leUser->font();
    // font.
    // ui->leUser->setText("User");
    // ui->leUser->setFont(font);
    // ui->lePassword->setText("Password");

    ui->rbDeviceApps->setChecked(true);

    ui->tblAccounts->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->tblAccounts, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnShowMenu(QPoint)));

    DisableAccountAdding(true);

    this->setWindowIcon(QIcon(":/icons/manager.png"));

}

void Manager::CreateCustomMenu()
{
    m_customMenu = new QMenu(this);

    QAction* copyAccount = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy), "Copy", this);
    QAction* editAccount = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::MailMessageNew), "Edit", this);
    QAction* removeAccount = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete), "Remove", this);

    connect(copyAccount, SIGNAL(triggered()), this, SLOT(OnCopyAccount()));
    connect(editAccount, SIGNAL(triggered()), this, SLOT(OnEditAccount()));
    connect(removeAccount, SIGNAL(triggered()), this, SLOT(OnRemoveAccount()));

    m_customMenu->addAction(copyAccount);
    m_customMenu->addAction(editAccount);
    m_customMenu->addAction(removeAccount);
}

void Manager::DisableAccountAdding(bool bDisable)
{
    ui->pbAddAccount->setDisabled(bDisable);
    ui->leUser->setDisabled(bDisable);
    ui->lePassword->setDisabled(bDisable);
    ui->pbPasswordVisible->setDisabled(bDisable);
    ui->pbDeleteApp->setDisabled(bDisable);
}

void Manager::LoadAccounts()
{
    m_saverLoader.LoadAccounts(m_deviceApps, m_otherApps);
    SetComboBoxItems(m_deviceApps);
    ui->cbApp->setCurrentIndex(-1);
}

void Manager::SetComboBoxItems(const AppsManager& appsManager)
{
	ui->cbApp->clear();

    for (auto sAppName : appsManager.GetAppNames())
    {
        ui->cbApp->insertItem(0, sAppName, appsManager.GetAppDescription(sAppName));
        ui->cbApp->setItemData(0, appsManager.GetAppDescription(sAppName), Qt::ToolTipRole);
        
    }
       
    ui->cbApp->setCurrentIndex(0);
}

void Manager::AddAppFromDevice()
{
    QString sFilter("Executable files (*.exe)");
    QFileDialog fdlg(this, "Select application executable file", "/C:/", sFilter);
    fdlg.setFileMode(QFileDialog::ExistingFile);

    if(!fdlg.exec())
        return;

    QStringList fileNames = fdlg.selectedFiles();

    for (auto filePath : fileNames)
    {
        QFileInfo fInfo(filePath);
        QString fName(fInfo.baseName());

        if (!m_deviceApps.AddApp(fName, filePath))
        {
            QMessageBox::information(this, "The application has already been added",
                "The selected application has already been added. You can add a new account for this application.");

			ui->cbApp->setCurrentIndex(ui->cbApp->findText(fName));
            break;
        }
    }

    SetComboBoxItems(m_deviceApps);
    m_bChanges = true;
}

void Manager::AddOtherApp()
{
	AddOtherAppDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted)
    {
		QString sAppName = dlg.GetAppName();
		QString sAppDescription = dlg.GetAppDescription();

        if (!m_otherApps.AddApp(sAppName, sAppDescription))
        {
            QMessageBox::information(this, "The application has already been added",
                "The selected application has already been added. You can add a new account for this application.");

            ui->cbApp->setCurrentIndex(ui->cbApp->findText(sAppName));
        }

        SetComboBoxItems(m_otherApps);
        m_bChanges = true;        
    }
    //m_leOtherAppName->show();
    //ui->cbApp->hide();

    //ui->cbApp->setEditable(true);
}

void Manager::UpdateAccountsTable(QString sAppName)
{
    UpdateAccountsTableForGroup(sAppName, ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::UpdateAccountsTableForGroup(const QString& sAppName, const AppsManager& appsManager)
{
	QMap<QString, QString> appAccounts = appsManager.GetAppAccounts(sAppName);

    ui->tblAccounts->clearContents();
    ui->tblAccounts->setRowCount(appAccounts.size());

    QIcon icon(":/icons/hide_light.png");

    int nRowCount = 0;
    for (auto sUser : appAccounts.keys())
    {
        QTableWidgetItem* twiUser = new QTableWidgetItem(sUser);
        ui->tblAccounts->setItem(nRowCount, COL_USER, twiUser);

        QTableWidgetItem* twiPassword = new QTableWidgetItem(QString::QString(appAccounts.value(sUser).size(), '*'));
        twiPassword->setData(Qt::UserRole, appAccounts.value(sUser));
        twiPassword->setCheckState(Qt::Checked);
        ui->tblAccounts->setItem(nRowCount, COL_PASSWORD, twiPassword);

        nRowCount++;
    }
}

void Manager::closeEvent(QCloseEvent* event)
{
    if (!m_bChanges)
    {
        event->accept(); 
        return;
    }

    QMessageBox msbSave(QMessageBox::Question,
        "Save changes",
        "Do you want to save changes?",
        QMessageBox::Yes | QMessageBox::No);

    int res = msbSave.exec();
    if (res == QMessageBox::No)
    {
        event->accept();
        return;
    }
    else if (res == QMessageBox::Yes)
    {
        bool bSaved = m_saverLoader.SaveAccounts(m_deviceApps, m_otherApps);

        if (!bSaved)
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
    else
    {
        event->ignore();
        return;
    }
}

void Manager::AddAccountForGroup(AppsManager& appsManager)
{
    if (ui->leUser->text().isEmpty() || ui->lePassword->text().isEmpty())
    {
        QMessageBox::information(this, "Some fields are not filled in",
            "The username or password isn't filled in. Fill in the missing fields.", QMessageBox::Ok);
        return;
    }

    QString sAppName = ui->cbApp->currentText();

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

    if (indCurrentApp != -1)
    {
        QMessageBox msb(QMessageBox::Question,
            "Remove application",
            "Are you sure you want to remove the application from the list and all accounts added to it?",
            QMessageBox::Yes | QMessageBox::No);

        if (msb.exec() != QMessageBox::Yes)
        {
            return;
        }

        QString sAppName = ui->cbApp->currentText();
		appsManager.RemoveApp(sAppName);

        ui->cbApp->removeItem(indCurrentApp);
        ui->cbApp->setCurrentIndex(-1);

        UpdateAccountsTable();
        m_bChanges = true;
    }
}

void Manager::RemoveAccountFromGroup(AppsManager& appsManager)
{
    if (!ui->tblAccounts->currentItem())
        return;

    QMessageBox msb(QMessageBox::Question,
        "Remove account",
        "Are you sure you want to remove the current account?",
        QMessageBox::Yes | QMessageBox::No);

    if (msb.exec() != QMessageBox::Yes)
    {
        return;
    }

    QString sUserName(ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_USER)->text());

	appsManager.RemoveAppAccount(ui->cbApp->currentText(), sUserName);
    
    ui->tblAccounts->removeRow(ui->tblAccounts->currentRow());
    m_bChanges = true;
}

void Manager::EditAccountInGroup(AppsManager& appsManager)
{
    if (!ui->tblAccounts->currentItem())
        return;

    EditDialog dlg(ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_USER)->text(),
        ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_PASSWORD)->data(Qt::UserRole).toString(), this);

    if (dlg.exec() == QDialog::Accepted)
    {
        QTableWidgetItem* currUsername = ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_USER);
        QTableWidgetItem* currPassword = ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_PASSWORD);

        QString sNewUsername = dlg.GetUsername();
        QString sNewPassword = dlg.GetPassword();

        if (!currUsername || !currPassword ||
            (currUsername->text() == sNewUsername &&
             currPassword->data(Qt::UserRole).toString() == sNewPassword))
            return;

        QString sAppName(ui->cbApp->currentText());
		appsManager.RemoveAppAccount(sAppName, currUsername->text());
		appsManager.AddAppAccount(sAppName, sNewUsername, sNewPassword);

        currUsername->setText(sNewUsername);
        currPassword->setData(Qt::UserRole, sNewPassword);
        OnChangeTableItemVisible(currPassword);

        m_bChanges = true;
    }
}

void Manager::OnChangeCurrentApp()
{
    if (ui->cbApp->currentIndex() == -1)
    {
        DisableAccountAdding(true);
        return;
    }

    DisableAccountAdding(false);

    QString sCurrentAppName = ui->cbApp->currentText();

    UpdateAccountsTable(sCurrentAppName);
}

void Manager::OnAddAccaunt()
{
    AddAccountForGroup(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::OnAddApp()
{
    if (ui->rbDeviceApps->isChecked())
    {
        AddAppFromDevice();
    }
    else if (ui->rbOtherApps->isChecked())
    {
        AddOtherApp();
    }

    DisableAccountAdding(ui->cbApp->currentIndex() == -1);
}

void Manager::OnRemoveApp()
{
    RemoveAppFromGroup(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::OnChangeTableItemVisible(QTableWidgetItem* ptwi)
{
    if (ptwi && ptwi->column() == COL_PASSWORD)
    {
        if (ptwi->checkState() == Qt::Checked)
        {
            ptwi->setText(QString::QString(ptwi->text().size(), '*'));
        }
        else
        {
            ptwi->setText(ptwi->data(Qt::UserRole).toString());
        }
    }
}

void Manager::OnChangePasswordVisible()
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

void Manager::OnShowMenu(QPoint pos)
{
    if (!ui->tblAccounts->itemAt(pos))
        return;

    m_customMenu->popup(ui->tblAccounts->viewport()->mapToGlobal(pos));
}

void Manager::OnEditAccount()
{
    EditAccountInGroup(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::OnRemoveAccount()
{
    RemoveAccountFromGroup(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
}

void Manager::OnCopyAccount()
{
    QTableWidgetItem* currItem = ui->tblAccounts->currentItem();
    if (!currItem)
        return;

    QClipboard* clipboard = QApplication::clipboard();

    if (currItem->column() == COL_USER)
        clipboard->setText(currItem->text());
    else if (currItem->column() == COL_PASSWORD)
        clipboard->setText(currItem->data(Qt::UserRole).toString());
}

void Manager::OnChangeGroup()
{
    SetComboBoxItems(ui->rbDeviceApps->isChecked() ? m_deviceApps : m_otherApps);
	ui->cbApp->setCurrentIndex(-1);
    UpdateAccountsTable();
}

