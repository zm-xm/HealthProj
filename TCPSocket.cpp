#include "TCPSocket.h"

TCPSocket::TCPSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &TCPSocket::readyRead,
            this, &TCPSocket::receiveMessage);
}

bool TCPSocket::connectToServer(QString strIp, quint16 port)
{
    bool ret = true;
    this->abort();
    this->connectToHost(QHostAddress(strIp), port);
    ret = this->waitForConnected();
    return ret;

}

bool TCPSocket::SendMessage(QByteArray content)
{
    int nwrite = this->write(content);
    if (nwrite  == -1)
    {
        qDebug() << "send fail!";
        return false;
    }
    else
    {
        qDebug() << "send sucess:" << nwrite;
        return true;
    }

}


// 接收数据
void TCPSocket::receiveMessage()
{
    QByteArray ba;
    ba.resize(this->bytesAvailable());

    int nread = this->read(ba.data(), ba.size());

    if (-1 == nread)
    {
        qDebug() << "receive fail!";
    }
    else
    {
        qDebug() << "receive sucess:" << nread;
        emit dealMessage(ba);
    }

}
