#ifndef MANAGER_H
#define MANAGER_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QCloseEvent>
#include <QMenu>
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

private slots:
    void OnChangeCurrentApp();
    void OnAddAccaunt();
    void OnAddApp();
    void OnDeleteApp();
    void OnChangeTableItemVisible(QTableWidgetItem* ptwi);
    void OnChangePasswordVisible();

    void OnShowMenu(QPoint pos);
    void OnEditAccount();
    void OnDeleteAccount();
    void OnCopyAccount();
    void OnChangeGroup();

private:
    void SetManagerUi();
    void CreateCustomMenu();
    void DisableAccountAdding(bool bDisable);
    void LoadAccounts();
    void AddComboBoxItems(const QStringList& fileNames, bool bClear = false);

    void AddAppFromDevice();
    void AddOtherApp();
    void ChangeCurrentApp();

    void UpdateAccountsTable(QString sAppData = QString());
	void UpdateAccountsTableForGroup(const QString& sAppData, const QMap<QString, QMap<QString, QString>>& groupApps);
    void closeEvent(QCloseEvent* event);

    void AddAccountForGroup(QMap<QString, QMap<QString, QString>>& groupApps);
    void DeleteAppFromGroup(QMap<QString, QMap<QString, QString>>& groupApps);
    void DeleteAccountFromGroup(QMap<QString, QMap<QString, QString>>& groupApps);
	void EditAccountInGroup(QMap<QString, QMap<QString, QString>>& groupApps);


private:
    Ui::Manager *ui;
    //QLineEdit* m_leOtherAppName;

    // < app path, < user, password > >
    QMap<QString, QMap<QString, QString>> m_deviceAppsData;
    QMap<QString, QMap<QString, QString>> m_otherAppsData;
    AccountsSaverLoader m_saverLoader;
    bool m_bChanges;

    QMenu* m_customMenu;
};

enum TableColumns
{
    COL_USER = 0,
    COL_PASSWORD,
    COL_VISIBLE
};
#endif // MANAGER_H
