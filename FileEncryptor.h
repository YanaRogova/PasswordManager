#pragma once
#include <QFile>
#include <QCryptographicHash>
class FileEncryptor
{
public:
    static bool encryptFile(const QString& inputFilePath, const QString& outputFilePath, const QByteArray& key);

    static bool decryptFile(const QString& inputFilePath, const QString& outputFilePath, const QByteArray& key);
};

