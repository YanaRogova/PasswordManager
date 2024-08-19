#include "FileEncryptor.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

bool FileEncryptor::encryptFile(const QString& inputFilePath, const QString& outputFilePath, const QByteArray& key) {
    
    QFile inputFile(inputFilePath);
    QFile outputFile(outputFilePath);

    //open files
    if (!inputFile.open(QIODevice::ReadOnly) || !outputFile.open(QIODevice::WriteOnly)) {
        return false;
    }

	// generate random initialization vector and write it to the output file
    QByteArray iv(EVP_MAX_IV_LENGTH, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), EVP_MAX_IV_LENGTH);
    outputFile.write(iv);

	// initialize encryption context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    QByteArray buffer;
    QByteArray encryptedBuffer;
    int outLen = 0;

	// encrypt the input file in blocks and write the encrypted blocks to the output file
    while (!(buffer = inputFile.read(1024)).isEmpty()) {
        encryptedBuffer.resize(buffer.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
        if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(encryptedBuffer.data()), &outLen, reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        outputFile.write(encryptedBuffer.constData(), outLen);
    }

	// finish encryption and write the final block
    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(encryptedBuffer.data()), &outLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    outputFile.write(encryptedBuffer.constData(), outLen);

    EVP_CIPHER_CTX_free(ctx);
    inputFile.close();
    outputFile.close();
    return true;
}

bool FileEncryptor::decryptFile(const QString& inputFilePath, const QString& outputFilePath, const QByteArray& key) {
    QFile inputFile(inputFilePath);
    QFile outputFile(outputFilePath);

    //open files
    if (!inputFile.open(QIODevice::ReadOnly) || !outputFile.open(QIODevice::WriteOnly)) {
        return false;
    }

	// read the initialization vector from the input file
    QByteArray iv = inputFile.read(EVP_MAX_IV_LENGTH);

	// initialize decryption context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    QByteArray buffer;
    QByteArray decryptedBuffer;
    int outLen = 0;

	// decrypt the input file in blocks and write the decrypted blocks to the output file
    while (!(buffer = inputFile.read(1024)).isEmpty()) {
        decryptedBuffer.resize(buffer.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
        if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(decryptedBuffer.data()), &outLen, reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        outputFile.write(decryptedBuffer.constData(), outLen);
    }

	// finish decryption and write the final block
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(decryptedBuffer.data()), &outLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    outputFile.write(decryptedBuffer.constData(), outLen);

    EVP_CIPHER_CTX_free(ctx);
    inputFile.close();
    outputFile.close();
    return true;
}