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
    connect(ui->pbDeleteApp, SIGNAL(clicked()), this, SLOT(OnDeleteApp()));
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
    QAction* deleteAccount = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete), "Delete", this);

    connect(copyAccount, SIGNAL(triggered()), this, SLOT(OnCopyAccount()));
    connect(editAccount, SIGNAL(triggered()), this, SLOT(OnEditAccount()));
    connect(deleteAccount, SIGNAL(triggered()), this, SLOT(OnDeleteAccount()));

    m_customMenu->addAction(copyAccount);
    m_customMenu->addAction(editAccount);
    m_customMenu->addAction(deleteAccount);
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
    m_saverLoader.LoadAccounts(m_deviceAppsData, m_otherAppsData);
    AddComboBoxItems(m_deviceAppsData.keys());
    ui->cbApp->setCurrentIndex(-1);
}

void Manager::AddComboBoxItems(const QStringList& fileNames, bool bClear)
{
    if (bClear)
		ui->cbApp->clear();

    for (auto filePath : fileNames)
    {
        if (ui->cbApp->findData(filePath) != -1)
        {
            ui->cbApp->setCurrentIndex(ui->cbApp->findData(filePath));
            QMessageBox::information(this, "The application has already been added",
                "The selected application has already been added. You can add a new account for this application.");
            continue;
        }

        QFileInfo fInfo(filePath);
        QString fName(fInfo.baseName());
        //disconnect(ui->cbApp, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeCurrentApp()));
        ui->cbApp->insertItem(0, fName, filePath);
        ui->cbApp->setItemData(0, filePath, Qt::ToolTipRole);
        
        ui->cbApp->setCurrentIndex(0);
        //connect(ui->cbApp, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeCurrentApp()));

        //TODO: get app name from path, add to cb
    }
}

void Manager::AddAppFromDevice()
{
     QString sFilter("Executable files (*.exe)");
    QFileDialog fdlg(this, "Select application executable file", "/C:/", sFilter);
    fdlg.setFileMode(QFileDialog::ExistingFile);

    if(!fdlg.exec())
        return;

    QStringList fileNames = fdlg.selectedFiles();
    AddComboBoxItems(fileNames);

    for (auto filePath : fileNames)
        m_deviceAppsData.insert(filePath, QMap<QString, QString>());

    m_bChanges = true;
}

void Manager::AddOtherApp()
{
	AddOtherAppDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted)
    {
        /*QTableWidgetItem* currUsername = ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_USER);
        QTableWidgetItem* currPassword = ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_PASSWORD);

        QString sNewUsername = dlg.GetUsername();
        QString sNewPassword = dlg.GetPassword();

        if (!currUsername || !currPassword ||
            (currUsername->text() == sNewUsername &&
                currPassword->data(Qt::UserRole).toString() == sNewPassword))
            return;

        QString sApp(ui->cbApp->currentData(Qt::UserRole).toString());
        groupApps[sApp].remove(currUsername->text());
        groupApps[sApp].insert(sNewUsername, sNewPassword);

        currUsername->setText(sNewUsername);
        currPassword->setData(Qt::UserRole, sNewPassword);
        OnChangeTableItemVisible(currPassword);

        m_bChanges = true;*/
    }
    //m_leOtherAppName->show();
    //ui->cbApp->hide();

    //ui->cbApp->setEditable(true);
}

void Manager::ChangeCurrentApp()
{

}

void Manager::UpdateAccountsTable(QString sAppData)
{
    UpdateAccountsTableForGroup(sAppData, ui->rbDeviceApps->isChecked() ? m_deviceAppsData : m_otherAppsData);
}

void Manager::UpdateAccountsTableForGroup(const QString& sAppData, const QMap<QString, QMap<QString, QString>>& groupApps)
{
    ui->tblAccounts->clearContents();
    ui->tblAccounts->setRowCount(groupApps.value(sAppData).keys().size());

    if (!groupApps.contains(sAppData))
        return;

    QIcon icon(":/icons/hide_light.png");

    int nRowCount = 0;
    for (auto sUser : groupApps.value(sAppData).keys())
    {
        QTableWidgetItem* twiUser = new QTableWidgetItem(sUser);
        ui->tblAccounts->setItem(nRowCount, COL_USER, twiUser);

        QTableWidgetItem* twiPassword = new QTableWidgetItem(QString::QString(groupApps.value(sAppData).value(sUser).size(), '*'));
        twiPassword->setData(Qt::UserRole, groupApps.value(sAppData).value(sUser));
        ui->tblAccounts->setItem(nRowCount, COL_PASSWORD, twiPassword);
        twiPassword->setCheckState(Qt::Checked);

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
        bool bSaved = m_saverLoader.SaveAccounts(m_deviceAppsData, m_otherAppsData);

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

void Manager::AddAccountForGroup(QMap<QString, QMap<QString, QString>>& groupApps)
{
    QString sAppData = ui->cbApp->currentData().toString();
    if (!groupApps.contains(sAppData))
        return;

    if (groupApps.value(sAppData).contains((ui->leUser->text())))
    {
        QMessageBox msb(QMessageBox::Question, "The specified username already exists",
            "The specified username already exists. Do you want to update the password for the specified username?",
            QMessageBox::Yes | QMessageBox::No);

        if (msb.exec() != QMessageBox::Yes)
            return;
    }

    if (ui->leUser->text().isEmpty() || ui->lePassword->text().isEmpty())
    {
        QMessageBox::information(this, "Some fields are not filled in",
            "The username or password isn't filled in. Fill in the missing fields.", QMessageBox::Ok);
        return;
    }

    groupApps[sAppData].insert(ui->leUser->text(), ui->lePassword->text());
    UpdateAccountsTable(sAppData);

    ui->leUser->clear();
    ui->lePassword->clear();
    m_bChanges = true;
}

void Manager::DeleteAppFromGroup(QMap<QString, QMap<QString, QString>>& groupApps)
{
    int indCurrentApp = ui->cbApp->currentIndex();
    if (indCurrentApp != -1)
    {
        QMessageBox msb(QMessageBox::Question,
            "Delete application",
            "Are you sure you want to delete the application from the list and all accounts added to it?",
            QMessageBox::Yes | QMessageBox::No);

        if (msb.exec() != QMessageBox::Yes)
        {
            return;
        }

        QString sApp = ui->cbApp->itemData(ui->cbApp->currentIndex(), Qt::UserRole).toString();
        groupApps.remove(sApp);
        ui->cbApp->removeItem(indCurrentApp);
        ui->cbApp->setCurrentIndex(-1);
        UpdateAccountsTable();
        m_bChanges = true;
    }
}

void Manager::DeleteAccountFromGroup(QMap<QString, QMap<QString, QString>>& groupApps)
{
    if (!ui->tblAccounts->currentItem())
        return;

    QMessageBox msb(QMessageBox::Question,
        "Delete account",
        "Are you sure you want to remove the current account?",
        QMessageBox::Yes | QMessageBox::No);

    if (msb.exec() != QMessageBox::Yes)
    {
        return;
    }

    QString sUserName(ui->tblAccounts->item(ui->tblAccounts->currentRow(), COL_USER)->text());
    QString sApp(ui->cbApp->currentData(Qt::UserRole).toString());

    groupApps[sApp].remove(sUserName);
    ui->tblAccounts->removeRow(ui->tblAccounts->currentRow());
    m_bChanges = true;
}

void Manager::EditAccountInGroup(QMap<QString, QMap<QString, QString>>& groupApps)
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

        QString sApp(ui->cbApp->currentData(Qt::UserRole).toString());
        groupApps[sApp].remove(currUsername->text());
        groupApps[sApp].insert(sNewUsername, sNewPassword);

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

    QString sCurrentAppData = ui->cbApp->currentData().toString();

    ChangeCurrentApp();
    UpdateAccountsTable(sCurrentAppData);
}

void Manager::OnAddAccaunt()
{
    AddAccountForGroup(ui->rbDeviceApps->isChecked() ? m_deviceAppsData : m_otherAppsData);
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

void Manager::OnDeleteApp()
{
    DeleteAppFromGroup(ui->rbDeviceApps->isChecked() ? m_deviceAppsData : m_otherAppsData);
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
    EditAccountInGroup(ui->rbDeviceApps->isChecked() ? m_deviceAppsData : m_otherAppsData);
}

void Manager::OnDeleteAccount()
{
	DeleteAccountFromGroup(ui->rbDeviceApps->isChecked() ? m_deviceAppsData : m_otherAppsData);
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
	AddComboBoxItems(ui->rbDeviceApps->isChecked() ? m_deviceAppsData.keys() : m_otherAppsData.keys(), true);
	ui->cbApp->setCurrentIndex(-1);
    UpdateAccountsTable();
}

