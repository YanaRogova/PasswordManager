#ifndef MANAGER_H
#define MANAGER_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QCloseEvent>
#include <QMenu>
#include "AccountsSaverLoader.h"
#include "AppsManager.h"


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
    void OnRemoveApp();
    void OnChangeTableItemVisible(QTableWidgetItem* ptwi);
    void OnChangePasswordVisible();

    void OnShowMenu(QPoint pos);
    void OnEditAccount();
    void OnRemoveAccount();
    void OnCopyAccount();
    void OnChangeGroup();

private:
    void SetManagerUi();
    void CreateCustomMenu();
    void DisableAccountAdding(bool bDisable);
    void LoadAccounts();
    void SetComboBoxItems(const AppsManager& appsManager);

    void AddAppFromDevice();
    void AddOtherApp();

    void UpdateAccountsTable(QString sAppName = QString());
	void UpdateAccountsTableForGroup(const QString& sAppName, const AppsManager& appsManager);
    void closeEvent(QCloseEvent* event);

    void AddAccountForGroup(AppsManager& appsManager);
    void RemoveAppFromGroup(AppsManager& appsManager);
    void RemoveAccountFromGroup(AppsManager& appsManager);
	void EditAccountInGroup(AppsManager& appsManager);


private:
    Ui::Manager *ui;
    //QLineEdit* m_leOtherAppName;

    // < app path, < user, password > >
	AppsManager m_deviceApps;
	AppsManager m_otherApps;

    //QMap<QString, QMap<QString, QString>> m_deviceAppsData;
    //QMap<QString, QMap<QString, QString>> m_otherAppsData;
    AccountsSaverLoader m_saverLoader;
    bool m_bChanges;

    QMenu* m_customMenu;

    QIcon m_iconHide;
	QIcon m_iconShow;
};

enum TableColumns
{
    COL_USER = 0,
    COL_PASSWORD
};
#endif // MANAGER_H
