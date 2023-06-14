#include "server.h"


Server::Server()
{
    uint port = 2323;
    if(this->listen(QHostAddress::Any, port))
    {
        qDebug() << "start";
        Data.clear();
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
    //connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::disconnected, this, &Server::disconn);

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
            in >> type;
            qDebug() << "type = " << type;
            QString str;
            QTime time;
            in >> time >> str;
            nextBlockSize = 0;
            SendToClient(str, type);
            qDebug() << str;
            break;
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

void Server::disconn()
{
    auto ind = Sockets.indexOf(socket);
    Sockets.removeAt(ind);
    socket->deleteLater();
}

void Server::SendToClient(QString str, quint16 type)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    if(type != 0)
    {
        out << quint16(0) << quint16(type) << QTime::currentTime() << SolveSystem(str, (int)type);
        out.device()->seek(0);
        out <<quint16(Data.size() - sizeof(quint16));
        socket->write(Data);
    }
    else
    {
        out << qint16(0) << quint16(type) << QTime::currentTime() << str;
        out.device()->seek(0);
        out <<quint16(Data.size() - sizeof(quint16));
        for(int i = 0; i < Sockets.size(); i++)
        {
            Sockets[i]->write(Data);
        }
    }
}

QString Server::SolveSystem(QString str, int n)
{
    bool flag = false;
    int temp = 0, j = 0, k = 0;
    int masA[n][n];
    int masB[n];
    for(int i = 0; i < (str.size() - 1); i++)
    {
        temp = temp * 10 + str[i].digitValue();
        if(!flag)
        {
            if(str[i+1] == ' ')
            {
                if(k != n)
                {
                    masA[j][k] = temp;
                    k++;
                }
                else
                {
                    masA[++j][0] = temp;
                    k = 1;
                }
                temp = 0;
                i++;
            }
            else if(str[i+1] == '|')
            {
                masA[n-1][n-1] = temp;
                temp =0;
                i++;
                flag = true;
                k = 0;
            }
        }
        else
        {
            if(str[i+1] == ' ')
            {
                masB[k++] = temp;
                temp = 0;
                i++;
            }
        }
    }
    QString ans = "";
    double res[n];
        for (int i = 0; i < n; i++)
        {
            res[i] = (double)masB[i] / masA[i][i];
            qDebug() << res[i];
        }

        double eps = 0.0001;
        double Xn[n];
        int num_of_iter = 0;

        do {
            num_of_iter++;
            for (int i = 0; i < n; i++) {
                Xn[i] = (double)masB[i] / masA[i][i];
                for (j = 0; j < n; j++) {
                    if (i == j)
                        continue;
                    else {
                        Xn[i] -= (double)masA[i][j] / masA[i][i] * res[j];
                    }
                }
            }

            bool flag = true;
            for (int i = 0; i < n - 1; i++) {
                if (dabs(Xn[i] - res[i]) > eps) {
                    flag = false;
                    break;
                }
            }

            for (int i = 0; i < n; i++) {
                res[i] = Xn[i];
            }

            if (flag)
                break;
        } while (num_of_iter < 1000);

    for (int i = 0; i < n; i++)
        ans.push_back(QString::number(res[i], 'f', 3) + ' ');
    return ans;

}

double Server::dabs(double val)
{
    double v;
    if(val < 0)
    {
        v = val*(-1);
    }
    else
    {
       v = val;
    }
    return v;
}
