#ifndef MANAGER_H
#define MANAGER_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QCloseEvent>
#include "AccountsSaverLoader.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QListWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class Manager;
}
QT_END_NAMESPACE

class Manager : public QMainWindow
{
    Q_OBJECT

public:
    Manager(QWidget *parent = nullptr);
    ~Manager();

public slots:
    void OnChangeCurrentApp();
    void OnAddAccaunt();
    void OnAddApp();
    void OnChangeTableItemVisible(QTableWidgetItem* ptwi);
    void OnChangePasswordVisible();

protected:
    void SetManagerUi();
    void DisableAccountAdding(bool bDisable);
    void LoadAccounts();
    void AddComboBoxItems(const QStringList& fileNames);

    void AddAppFromDevice();
    void AddOtherApp();
    void ChangeCurrentApp();

    void UpdateAccountsTable(QString sAppData);
    void closeEvent(QCloseEvent* event);

private:
    Ui::Manager *ui;
    //QLineEdit* m_leOtherAppName;

    // < app path, < user, password > >
    QMap<QString, QMap<QString, QString>> m_managerData;
    AccountsSaverLoader m_saverLoader;
};

enum TableColumns
{
    COL_USER = 0,
    COL_PASSWORD,
    COL_VISIBLE
};
#endif // MANAGER_H
