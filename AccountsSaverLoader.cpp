#include "AccountsSaverLoader.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>
#include "FileEncryptor.h"

AccountsSaverLoader::AccountsSaverLoader(QString sPath)
	: m_sPath(sPath), m_sFileName("accounts.json"), m_sBufFileName("temp.json"), m_hashKey("yourpassword")
{
}

bool AccountsSaverLoader::SaveAccounts(const AppsManager& deviceAppsData,
	const AppsManager& otherAppsData)
{
	QJsonObject jsonObject = ObjectToSave(deviceAppsData, otherAppsData);

	QDir::setCurrent(m_sPath);
	QFile jsonBufFile(m_sBufFileName);
	QFile jsonFile(m_sFileName);

	if (!jsonBufFile.open(QIODevice::WriteOnly))
	{
		return false;
	}

	jsonBufFile.write(QJsonDocument(jsonObject).toJson(QJsonDocument::Indented));
	jsonBufFile.close();

	QByteArray key = QCryptographicHash::hash(m_hashKey, QCryptographicHash::Sha256);

	FileEncryptor::encryptFile(jsonBufFile.fileName(), jsonFile.fileName(), key);

	jsonBufFile.remove();

	return true;
}

void AccountsSaverLoader::LoadAccounts(AppsManager& deviceAppsData, AppsManager& otherAppsData)
{
	QDir::setCurrent(m_sPath);
	QFile jsonFile(m_sFileName);
	QFile jsonBufFile(m_sBufFileName);

	QByteArray key = QCryptographicHash::hash(m_hashKey, QCryptographicHash::Sha256);
	FileEncryptor::decryptFile(jsonFile.fileName(), jsonBufFile.fileName(), key);

	if (!jsonBufFile.open(QIODevice::ReadOnly))
	{
		return;
	}

	QByteArray loadData = jsonBufFile.readAll();
	QJsonDocument jsonDoc(QJsonDocument::fromJson(loadData));
	QJsonObject jsonObject(jsonDoc.object());

	LoadGroupsFromObject(jsonObject, deviceAppsData, otherAppsData);

	jsonBufFile.remove();
}

QJsonObject AccountsSaverLoader::ConvertAccountsToJson(const AppsManager& mAppsAccounts)
{
	QJsonArray apps;
	for (auto sApp : mAppsAccounts.GetAppNames())
	{
		QJsonObject app;
		app["app"] = sApp;
		app["description"] = mAppsAccounts.GetAppDescription(sApp);

		QJsonArray accounts;
		QMap<QString, QString> mapAccounts = mAppsAccounts.GetAppAccounts(sApp);

		for (auto sUser : mapAccounts.keys())
		{
			QJsonObject account;
			account["username"] = sUser;
			account["password"] = mapAccounts.value(sUser);

			accounts.append(account);
		}
		app["accounts"] = accounts;

		apps.append(app);
	}

	QJsonObject jsonObject;
	jsonObject["apps"] = apps;
	
	return jsonObject;
}

QJsonObject AccountsSaverLoader::ObjectToSave(const AppsManager& deviceAppsData, const AppsManager& otherAppsData)
{
	QJsonArray groups;

	QJsonObject groupDevice = ConvertAccountsToJson(deviceAppsData);
	groupDevice["group"] = "device";
	groups.append(groupDevice);

	QJsonObject groupOther = ConvertAccountsToJson(otherAppsData);
	groupOther["group"] = "other";
	groups.append(groupOther);

	QJsonObject jsonObject;
	jsonObject["groups"] = groups;

	return jsonObject;
}

AppsManager AccountsSaverLoader::ConvertAccountsFromJson(const QJsonObject& jsonObject)
{
	AppsManager appsManager;
	
	QJsonArray jaApps = jsonObject.value("apps").toArray();
	for (int i = 0; i < jaApps.count(); i++)
	{
		QMap<QString, QString> accounts;

		QJsonObject app = jaApps.at(i).toObject();			
		
		QJsonArray jaAccounts = app.value("accounts").toArray();
		
		for (int j = 0; j < jaAccounts.count(); j++)
		{
			QJsonObject account = jaAccounts.at(j).toObject();

			accounts.insert(account.value("username").toString(), account.value("password").toString());
		}

		QString sApp = app.value("app").toString();
		QString sDescription = app.value("description").toString();

		appsManager.AddApp(sApp, sDescription, accounts);
	}

	return appsManager;
}

void AccountsSaverLoader::LoadGroupsFromObject(const QJsonObject& jsonObject, AppsManager& deviceAppsData, AppsManager& otherAppsData)
{

	QJsonArray jaGroups = jsonObject.value("groups").toArray();
	for (int i = 0; i < jaGroups.count(); i++)
	{
		QJsonObject group = jaGroups.at(i).toObject();

		if (group.value("group").toString() == "device")
			deviceAppsData = ConvertAccountsFromJson(group);
		else if (group.value("group").toString() == "other")
			otherAppsData = ConvertAccountsFromJson(group);
	}
}
