#pragma once
#include <QMap>
#include <QWidget>
class AppsManager
{
public:
	bool AddApp(QString sAppName, QString sDescription, QMap<QString, QString> mapAccounts = QMap<QString, QString>());
	void RemoveApp(QString sAppName);
	bool AddAppAccount(QString sAppName, QString sUsername, QString sPassword);
	void RemoveAppAccount(QString sAppName, QString sUsername);

	QVector<QString> GetAppNames() const;

	QString GetAppDescription(QString sAppName) const;
	QMap<QString, QString> GetAppAccounts(QString sAppName) const;

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
	QWidget* m_parent;
};

