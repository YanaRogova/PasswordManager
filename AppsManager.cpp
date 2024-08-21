#include "AppsManager.h"
#include <QMessageBox>

bool AppsManager::AddApp(QString sAppName, QString sDescription, QMap<QString, QString> mapAccounts)
{
	if (m_mapApps.contains(sAppName)) 
	{
		return false;
	}
	
	AppInfo appInfo;
	appInfo.sDescription = sDescription;
	appInfo.mapAccounts = mapAccounts;

	m_mapApps.insert(sAppName, appInfo);
	return true;
}

void AppsManager::RemoveApp(QString sAppName)
{
	if (m_mapApps.contains(sAppName))
		m_mapApps.remove(sAppName);
}

bool AppsManager::AddAppAccount(QString sAppName, QString sUsername, QString sPassword)
{
	if (!m_mapApps.contains(sAppName))
		return false;

	if (m_mapApps[sAppName].mapAccounts.contains(sUsername))
	{
		QMessageBox msb(QMessageBox::Question, "The specified username already exists",
			"The specified username already exists. Do you want to update the password for the specified username?",
			QMessageBox::Yes | QMessageBox::No);

		if (msb.exec() != QMessageBox::Yes)
			return false;
	}

	m_mapApps[sAppName].mapAccounts.insert(sUsername, sPassword);
	return true;
}

void AppsManager::RemoveAppAccount(QString sAppName, QString sUsername)
{
	if (m_mapApps.contains(sAppName))
		m_mapApps[sAppName].mapAccounts.remove(sUsername);
}

QVector<QString> AppsManager::GetAppNames() const
{
	return m_mapApps.keys();
}

QString AppsManager::GetAppDescription(QString sAppName) const
{
	if (!m_mapApps.contains(sAppName))
		return QString();

	return m_mapApps.value(sAppName).sDescription;
}

QMap<QString, QString> AppsManager::GetAppAccounts(QString sAppName) const
{
	if (!m_mapApps.contains(sAppName))
		return QMap<QString, QString>();

	return m_mapApps.value(sAppName).mapAccounts;
}

void AppsManager::SetAppDescription(QString sAppName, QString sDescription)
{
	if (m_mapApps.contains(sAppName))
		m_mapApps[sAppName].sDescription = sDescription;
}

void AppsManager::SetAppAccounts(QString sAppName, QMap<QString, QString> mapAccounts)
{
	if (m_mapApps.contains(sAppName))
		m_mapApps[sAppName].mapAccounts = mapAccounts;
}
