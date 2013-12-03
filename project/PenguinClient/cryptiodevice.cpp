#include "cryptiodevice.h"
#include <QByteArray>

CryptIODevice::CryptIODevice(QIODevice *underlyingDevice, QObject *parent) :
    QIODevice(parent),
    m_underlyingDevice(underlyingDevice),
    m_blockSize(16)
{
}

CryptIODevice::~CryptIODevice(){
    close();
}

bool CryptIODevice::open(OpenMode mode){
    if(isOpen())
    {
        return false;
    }

    if((mode & ReadWrite) == ReadWrite)
        return false;

    OpenMode underlyingOpenMode = mode & ~QIODevice::Text;
    bool underlyingOk = false;
    if (m_underlyingDevice->isOpen())
        underlyingOk = (m_underlyingDevice->openMode() == underlyingOpenMode);
    else
        underlyingOk = m_underlyingDevice->open(underlyingOpenMode);

    if (underlyingOk)
    {
        setOpenMode(mode);
        return true;
    }
    return false;
}

void CryptIODevice::close()
{
    if(isOpen())
    {
        flushEnd();
        m_underlyingDevice->close();
        setOpenMode(NotOpen);
    }
}

bool CryptIODevice::isSequential() const
{
    return true;
}

void CryptIODevice::setkey(QString key){
    aes_setkey_enc(ctx, (unsigned char*) key.toStdString().c_str(), 128);
    resource = new Resource();
    preCalcThread = new PreCalc(this);
    preCalcThread->startThread(resource, ctx);

}

void CryptIODevice::flushEnd(){
    if(openMode() & WriteOnly)
    {
        quint64 realBytesWritten = writeBlock(m_byteBuffer);
        emit encryptedBytesWritten(realBytesWritten);
    }
}

qint64 CryptIODevice::writeData(const char* data, qint64 maxSize)
{
    m_byteBuffer.append(data, (int)maxSize);
    quint64 realBytesWritten = 0;

    // always write blocks of m_blockSize bytes !!!
    while(m_byteBuffer.size() > m_blockSize)
    {
        QByteArray bytesToWrite = m_byteBuffer.left(m_blockSize);
        m_byteBuffer.remove(0, m_blockSize);

        realBytesWritten += writeBlock(bytesToWrite);
    }

    emit encryptedBytesWritten(realBytesWritten);

    return maxSize;
}

int CryptIODevice::writeBlock(const QByteArray &bytesToWrite){

    QVector<unsigned char> toXor = resource->readData();
    QByteArray xored;
    xored.resize(bytesToWrite.size());
    for (int i = 0; i < bytesToWrite.size(); ++i){
        xored[i] = bytesToWrite[i] ^ toXor.at(i);
    }
    return m_underlyingDevice->write(xored);
}

qint64 CryptIODevice::readData(char *data, qint64 maxSize){
    int bytesRead = 0;
    if(!m_byteBuffer.isEmpty())
    {
        for(int copyByte = 0; copyByte < qMin(m_byteBuffer.size(), (int)maxSize); ++copyByte, ++bytesRead)
        {
            data[bytesRead] = m_byteBuffer[copyByte];
        }

        m_byteBuffer.remove(0, bytesRead);
    }
    while(m_byteBuffer.isEmpty() && (bytesRead < maxSize) && !m_underlyingDevice->atEnd())
    {
        int sizeOfCipher = 0;
        int bytesReallyRead = m_underlyingDevice->read((char*)&sizeOfCipher, sizeof(sizeOfCipher));

        if(bytesReallyRead != sizeof(sizeOfCipher))
            return -1;

        QByteArray myCipherText;
        myCipherText.resize(sizeOfCipher);
        bytesReallyRead = m_underlyingDevice->read(myCipherText.data(), sizeOfCipher);

        if(bytesReallyRead != bytesRead)
        {
            m_byteBuffer = decryptToByteArray(myCipherText);

            int copyByte = 0;
            for(copyByte = 0; (copyByte < m_byteBuffer.size()) && (bytesRead < (int)maxSize); ++copyByte, ++bytesRead)
            {
                data[bytesRead] = m_byteBuffer[copyByte];
            }
            m_byteBuffer.remove(0, copyByte);

        }
    }
    return bytesRead;
}

QByteArray CryptIODevice::decryptToByteArray(QByteArray in){
    QByteArray out;
    QVector<unsigned char> toXor;
    while(in.size() > m_blockSize)
    {
        QByteArray byteBlock = in.left(m_blockSize);
        in.remove(0, m_blockSize);

        toXor = resource->readData();
        QByteArray xored;
        xored.resize(byteBlock.size());
        for (int i = 0; i < byteBlock.size(); ++i){
            xored[i] = byteBlock[i] ^ toXor.at(i);
        }
        out.append(xored);
    }
    return out;
}















