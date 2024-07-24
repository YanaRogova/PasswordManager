#include "Manager.h"
#include "ui_Manager.h"
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>

Manager::Manager(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Manager)
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


    // connect signals
    connect(ui->cbApp, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeCurrentApp()));
    connect(ui->pbAddAccount, SIGNAL(clicked()), this, SLOT(OnAddAccaunt()));
    connect(ui->pbAddApp, SIGNAL(clicked()), this, SLOT(OnAddApp()));
    connect(ui->tblAccounts, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(OnChangeTableItemVisible(QTableWidgetItem*)));
    connect(ui->pbPasswordVisible, SIGNAL(clicked()), this, SLOT(OnChangePasswordVisible()));
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

    // set widjets to add new record
    //ui->pbAddAccount->setDisabled(true);
    //ui->leUser->setDisabled(true);
    //ui->lePassword->setDisabled(true);

    // set font
    ui->leUser->setPlaceholderText("Username");
    ui->lePassword->setPlaceholderText("Password");
    ui->lePassword->setEchoMode(QLineEdit::Password);
    // QFont font = ui->leUser->font();
    // font.
    // ui->leUser->setText("User");
    // ui->leUser->setFont(font);
    // ui->lePassword->setText("Password");

    ui->rbDeviceApps->setChecked(true);

    ui->pbAddAccount->setDisabled(true);
    ui->leUser->setDisabled(true);
    ui->lePassword->setDisabled(true);
    ui->pbPasswordVisible->setDisabled(true);
}

void Manager::AddAppFromDevice()
{
     QString sFilter("Executable files (*.exe)");
    QFileDialog fdlg(this, "Select application executable file", "/C:/", sFilter);
    fdlg.setFileMode(QFileDialog::ExistingFile);

    QStringList fileNames;
    if(fdlg.exec())
        fileNames = fdlg.selectedFiles();

    for (auto filePath : fileNames)
    {
        if (ui->cbApp->findData(filePath) != -1)
        {
            ui->cbApp->setCurrentIndex(ui->cbApp->findData(filePath));
            QMessageBox msb(this);
            msb.setWindowTitle("The application has already been added");
            msb.setText("The selected application has already been added. You can add a new account for this application.");
            msb.setIcon(QMessageBox::Information);
            msb.exec();
            continue;
        }

        QFileInfo fInfo(filePath);
        QString fName(fInfo.baseName());
        //disconnect(ui->cbApp, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeCurrentApp()));
        ui->cbApp->addItem(fName, filePath);
        qDebug() << ui->cbApp->count();
        ui->cbApp->setCurrentIndex(ui->cbApp->count() - 1);
        //connect(ui->cbApp, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeCurrentApp()));

        m_managerData.insert(filePath, QMap<QString, QString>());
        //TODO: get app name from path, add to cb
    }
}

void Manager::AddOtherApp()
{
    //m_leOtherAppName->show();
    //ui->cbApp->hide();

    //ui->cbApp->setEditable(true);
}

void Manager::ChangeCurrentApp()
{

}

void Manager::UpdateAccauntsTable(QString sAppData)
{
    ui->tblAccounts->clearContents();
    ui->tblAccounts->setRowCount(m_managerData.value(sAppData).keys().size());

    if(!m_managerData.contains(sAppData))
        return;

    int nRowCount = 0;
    for (auto sUser : m_managerData.value(sAppData).keys())
    {
        QTableWidgetItem *twiUser = new QTableWidgetItem(sUser);
        ui->tblAccounts->setItem(nRowCount, COL_USER, twiUser);

        QTableWidgetItem *twiPassword = new QTableWidgetItem(QString::QString(m_managerData.value(sAppData).value(sUser).size(), '*'));
        twiPassword->setData(Qt::UserRole, m_managerData.value(sAppData).value(sUser));
        ui->tblAccounts->setItem(nRowCount, COL_PASSWORD, twiPassword);

        QTableWidgetItem* twiVisible = new QTableWidgetItem("Hidden");
        ui->tblAccounts->setItem(nRowCount, COL_VISIBLE, twiVisible);

        nRowCount++;
    }
}

void Manager::OnChangeCurrentApp()
{
    if (ui->cbApp->currentIndex() == -1)
        return;

    QString sCurrentAppData = ui->cbApp->currentData().toString();

    ChangeCurrentApp();
    UpdateAccauntsTable(sCurrentAppData);
}

void Manager::OnAddAccaunt()
{
    QString sAppData = ui->cbApp->currentData().toString();
    if (!m_managerData.contains(sAppData))
        return;

    if (m_managerData.value(sAppData).contains((ui->leUser->text())))
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

    m_managerData[sAppData].insert(ui->leUser->text(), ui->lePassword->text());
    UpdateAccauntsTable(sAppData);

    ui->leUser->clear();
    ui->lePassword->clear();
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

    bool bNoApps = ui->cbApp->count() == 0;

    ui->pbAddAccount->setDisabled(bNoApps);
    ui->leUser->setDisabled(bNoApps);
    ui->lePassword->setDisabled(bNoApps);
    ui->pbPasswordVisible->setDisabled(bNoApps);
}

void Manager::OnChangeTableItemVisible(QTableWidgetItem* ptwi)
{
    if (ptwi && ptwi->column() == COL_VISIBLE)
    {
        QTableWidgetItem* ptwiPassword = ui->tblAccounts->item(ptwi->row(), COL_PASSWORD);

        if (ptwi->text() == "Hidden")
        {
            ptwi->setText("Visible");

            if (ptwiPassword)
                ptwiPassword->setText(ptwiPassword->data(Qt::UserRole).toString());
        }
        else
        {
            ptwi->setText("Hidden");
            if (ptwiPassword)
                ptwiPassword->setText(QString::QString(ptwiPassword->text().size(), '*'));
        }

    }
}

void Manager::OnChangePasswordVisible()
{
    if (ui->pbPasswordVisible->text() == "Hidden")
    {
        ui->pbPasswordVisible->setText("Visible");
        ui->lePassword->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->pbPasswordVisible->setText("Hidden");
        ui->lePassword->setEchoMode(QLineEdit::Password);
    }
}


