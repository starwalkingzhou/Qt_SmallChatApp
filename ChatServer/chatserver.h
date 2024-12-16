#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QTcpServer>
#include <serverworker.h>
class ChatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ChatServer(QObject *parent = nullptr);
    QVector<ServerWorker*>m_clients;
protected:
    void incomingConnection(qintptr socketDescriptor) override;
    void broadcast(const QJsonObject &message,ServerWorker *exclude);
public slots:
    void stopServer();
    void jsonReceieved(ServerWorker *sender,const QJsonObject &docObj);
    void userDisconnected(ServerWorker* sender);

signals:
    void logMessage(const QString& msg);
};



#endif // CHATSERVER_H
