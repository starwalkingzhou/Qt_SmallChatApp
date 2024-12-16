#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescripror(qintptr socketDescriptor);

    QString getUserName();
    void setUserName(QString user);
signals:
    void logMessage(const QString& msg);
    void jsonReceieved(ServerWorker *sender,const QJsonObject &docObj);
    void disconnectedFromClient();

private:
    QTcpSocket* m_serverSocket;
    QString userName;

public slots:
    void onReadyRead();
    void sendMessage(const QString&text,const QString &type="message");
    void sendJson(const QJsonObject &json);
};

#endif // SERVERWORKER_H
