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

public slots:
    void stopServer();

signals:
    void logMessage(const QString& msg);
};

#endif // CHATSERVER_H
