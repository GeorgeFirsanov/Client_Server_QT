#include "server.h"


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
    nextBlockSize = 0;
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
        while(true)
        {

            if(nextBlockSize == 0)
            {
                qDebug() << "nBS = 0";
                if(socket->bytesAvailable()<2)
                    break;
                in >> nextBlockSize;
                qDebug() << "nBS = " << nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize)
                break;
            QString str;
            QTime time;
            in >> time >> str;
            nextBlockSize = 0;
            SendToClient(str);
            qDebug() << str;
            break;
        }
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
    out << qint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out <<quint16(Data.size() - sizeof(quint16));
    for(int i = 0; i < Sockets.size(); i++)
    {
       Sockets[i]->write(Data);
    }
}
