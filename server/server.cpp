#include "server.h"
#include <QDataStream>

Server::Server()
{
    uint port = 2323;
    if(this->listen(QHostAddress::Any, port))
    {
        qDebug() << "start";
    }
    else
    {
        qDebug() << "error";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket ->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    Sockets.push_back(socket);
    qDebug() << "client connected" << socketDescriptor;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "read";
        QString str;
        in >> str;
        qDebug() << str;
        SendToClient(str);
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

void Server::SendToClient(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    out << str;
    for(int i = 0; i < Sockets.size(); i++)
    {
       Sockets[i]->write(Data);
    }
}
