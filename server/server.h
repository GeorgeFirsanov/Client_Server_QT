#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QtGlobal>
#include <QDataStream>
#include <QTime>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server();
    QTcpSocket* socket;

private:
    QVector <QTcpSocket*> Sockets;
    QByteArray Data;
    void SendToClient(QString str, quint16 type);
    quint16 nextBlockSize;
    quint16 type;
    QString SolveSystem(QString str, int n);
    double dabs(double val);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
    void disconn();

};

#endif // SERVER_H
