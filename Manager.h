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


private:
	// set parametr for ui elements
    void SetManagerUi();
    void CreateCustomMenu();

    // make enable/disable ui elements that used for adding new accounts
    void DisableAccountAdding(bool bDisable);
    void LoadAccounts();
    //set combobox items for the group of applications
    void SetComboBoxItems(const AppsManager& appsManager);

    void AddAppFromDevice();
    void AddOtherApp();

	// update accounts table based on current application
    void UpdateAccountsTable(QString sAppName = QString());
	void UpdateAccountsTableForGroup(const QString& sAppName, const AppsManager& appsManager);
    
    void closeEvent(QCloseEvent* event);

    void AddAccountForGroup(AppsManager& appsManager);
    void RemoveAppFromGroup(AppsManager& appsManager);
    void RemoveAccountFromGroup(AppsManager& appsManager);
	void EditAccountInGroup(AppsManager& appsManager);


private slots:
    // handle changes in combobox current item
    void OnChangeCurrentApp();

    void OnAddAccaunt();
    void OnAddApp();
    void OnRemoveApp();

    // handle password visibilty in the table
    void OnChangeTableItemVisible(QTableWidgetItem* ptwi);
	// handle password visibilty in the line edit for adding new account
    void OnChangePasswordVisible();

    void OnShowMenu(QPoint pos);

    void OnEditAccount();
    void OnRemoveAccount();
    void OnCopyAccount();
    void OnChangeGroup();

private:
    Ui::Manager *ui;

	AppsManager m_deviceApps;
	AppsManager m_otherApps;

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
