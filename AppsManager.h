#pragma once
#include <QMap>
#include <QWidget>
class AppsManager
{
public:
	// functions to add and remove apps and their accounts
	bool AddApp(QString sAppName, QString sDescription, QMap<QString, QString> mapAccounts = QMap<QString, QString>());
	void RemoveApp(QString sAppName);
	bool AddAppAccount(QString sAppName, QString sUsername, QString sPassword);
	void RemoveAppAccount(QString sAppName, QString sUsername);

	// functions to get app data
	QVector<QString> GetAppNames() const;
	QString GetAppDescription(QString sAppName) const;
	QMap<QString, QString> GetAppAccounts(QString sAppName) const;

	// functions to modify app data
	void SetAppDescription(QString sAppName, QString sDescription);
	void SetAppAccounts(QString sAppName, QMap<QString, QString> mapAccounts);

private:
	struct AppInfo
	{
		QString sDescription;
		// <key: username, value: password>
		QMap<QString, QString> mapAccounts;
	};

	QMap<QString, AppInfo> m_mapApps;
};

