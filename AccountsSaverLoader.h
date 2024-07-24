#pragma once

#include <QString>
#include <QJsonObject>

class AccountsSaverLoader
{
public:
	AccountsSaverLoader(QString sPath);

	bool SaveAccounts(const QMap<QString, QMap<QString, QString>>& mAppsAccounts);
	void LoadAccounts(QMap<QString, QMap<QString, QString>>& mAppsAccounts);

private:
	QJsonObject ConvertAccountsToJson(const QMap<QString, QMap<QString, QString>>& mAppsAccounts);
	QMap<QString, QMap<QString, QString>> ConvertAccountsFromJson(const QJsonObject& jsonObject);

private:
	QString m_sPath;
	QString m_sFileName;
};

