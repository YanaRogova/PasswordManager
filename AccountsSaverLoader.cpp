#include "AccountsSaverLoader.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>

AccountsSaverLoader::AccountsSaverLoader(QString sPath)
	: m_sPath(sPath), m_sFileName("accounts.json")
{
}

bool AccountsSaverLoader::SaveAccounts(const QMap<QString, QMap<QString, QString>>& mAppsAccounts)
{
	QJsonObject jsonObject = ConvertAccountsToJson(mAppsAccounts);

	QDir::setCurrent(m_sPath);
	QFile jsonFile(m_sFileName);

	if (!jsonFile.open(QIODevice::WriteOnly))
	{
		return false;
	}

	jsonFile.write(QJsonDocument(jsonObject).toJson(QJsonDocument::Indented));
	jsonFile.close();

	return true;
}

void AccountsSaverLoader::LoadAccounts(QMap<QString, QMap<QString, QString>>& mAppsAccounts)
{
	QDir::setCurrent(m_sPath);
	QFile jsonFile(m_sFileName);

	if (!jsonFile.open(QIODevice::ReadOnly))
	{
		return;
	}

	QByteArray loadData = jsonFile.readAll();
	QJsonDocument jsonDoc(QJsonDocument::fromJson(loadData));
	QJsonObject jsonObject(jsonDoc.object());

	mAppsAccounts = ConvertAccountsFromJson(jsonObject);
}

QJsonObject AccountsSaverLoader::ConvertAccountsToJson(const QMap<QString, QMap<QString, QString>>& mAppsAccounts)
{
	QJsonArray apps;
	for (auto sApp : mAppsAccounts.keys())
	{
		QJsonObject app;
		app["app"] = sApp;

		QJsonArray accounts;
		for (auto sUser : mAppsAccounts.value(sApp).keys())
		{
			QJsonObject account;
			account["username"] = sUser;
			account["password"] = mAppsAccounts.value(sApp).value(sUser);

			accounts.append(account);
		}
		app["accounts"] = accounts;

		apps.append(app);
	}

	QJsonObject jsonObject;
	jsonObject["apps"] = apps;
	
	return jsonObject;
}

QMap<QString, QMap<QString, QString>> AccountsSaverLoader::ConvertAccountsFromJson(const QJsonObject& jsonObject)
{
	QMap<QString, QMap<QString, QString>> mAppsAccounts;
	qDebug() << jsonObject;
	QJsonArray jaApps = jsonObject.value("apps").toArray();
	for (int i = 0; i < jaApps.count(); i++)
	{
		QMap<QString, QString> accounts;

		QJsonObject app = jaApps.at(i).toObject();			
		qDebug() << app;
		QJsonArray jaAccounts = app.value("accounts").toArray();
		qDebug() << jaAccounts;
		for (int j = 0; j < jaAccounts.count(); j++)
		{
			QJsonObject account = jaAccounts.at(j).toObject();

			accounts.insert(account.value("username").toString(), account.value("password").toString());
		}

		mAppsAccounts.insert(app.value("app").toString(), accounts);
	}

	return mAppsAccounts;
}
