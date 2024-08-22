#pragma once

#include <QString>
#include <QJsonObject>
#include "AppsManager.h"

class AccountsSaverLoader
{
public:
	AccountsSaverLoader(QString sPath);

	//save accounts to file
	bool SaveAccounts(const AppsManager& deviceAppsData, const AppsManager& otherAppsData);

	//load accounts from file
	void LoadAccounts(AppsManager& deviceAppsData, AppsManager& otherAppsData);

private:
	// convert accounts data for one group of applications to JSON object
	QJsonObject ConvertAccountsToJson(const AppsManager& mAppsAccounts);

	//create finally json object with accounts data for different groups of applications
	QJsonObject ObjectToSave(const AppsManager& deviceAppsData,	const AppsManager& otherAppsData);

	// convert JSON object to accounts data for one group of applications
	AppsManager ConvertAccountsFromJson(const QJsonObject& jsonObject);

	// load accounts data for different groups of applications from JSON object
	void LoadGroupsFromObject(const QJsonObject& jsonObject, AppsManager& deviceAppsData,
		AppsManager& otherAppsData);

private:
	QString m_sPath;
	QString m_sFileName;
	QString m_sBufFileName;
	QByteArrayView m_hashKey;
};

