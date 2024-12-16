#include "chatserver.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

ChatServer::ChatServer(QObject *parent)
    :QTcpServer(parent)
{

}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker(this);
    if(!worker->setSocketDescripror(socketDescriptor)){
        worker->deleteLater();
        return;
    }
    connect(worker,&ServerWorker::logMessage,this, &ChatServer::logMessage);
    connect(worker,&ServerWorker::jsonReceieved,this, &ChatServer::jsonReceieved);
    connect(worker,&ServerWorker::disconnectedFromClient,this,
            std::bind(&ChatServer::userDisconnected,this,worker));
    m_clients.append(worker);
    emit logMessage("用户连接上了");
}

void ChatServer::broadcast(const QJsonObject &message, ServerWorker *exclude)
{
    for(ServerWorker *worker:m_clients){
        worker->sendJson(message);
    }
}

void ChatServer::stopServer()
{
    close();
}

void ChatServer::jsonReceieved(ServerWorker *sender, const QJsonObject &docObj)
{
    const QJsonValue typeVal= docObj.value("type");
    if(typeVal.isNull()||!typeVal.isString())
        return;
    if(typeVal.toString().compare("message",Qt::CaseInsensitive)== 0){
        const QJsonValue textVal= docObj.value("text");
        if(textVal.isNull() || !textVal.isString())
            return;
        const QString text=textVal.toString().trimmed();
        if(text.isEmpty())
            return;
        QJsonObject message;
        message[QStringLiteral("type")]="message";
        message[QStringLiteral("text")]= text;
        message[QStringLiteral("sender")]=sender->getUserName();

        broadcast(message,sender);
    }else if(typeVal.toString().compare("login",Qt::CaseInsensitive)== 0){
        const QJsonValue usernameVal= docObj.value("text");
        if(usernameVal.isNull()||!usernameVal.isString())
            return;
        sender->setUserName(usernameVal.toString());
        QJsonObject connectedMessage;
        connectedMessage["type"]="newuser";
        connectedMessage["username"]= usernameVal.toString();
        broadcast(connectedMessage,sender);

        //send user list to new logineduser
        QJsonObject userListMessage;userListMessage["type"]="userlist";
        QJsonArray userlist;
        for(ServerWorker *worker :m_clients){
            if(worker == sender)
                userlist.append(worker->getUserName()+"*");
            else
                userlist.append(worker->getUserName());
        }
        userListMessage["userlist"]=userlist;
        sender->sendJson(userListMessage);
    }
}

void ChatServer::userDisconnected(ServerWorker *sender)
{
    m_clients.removeAll(sender);
    const QString userName = sender->getUserName();
    if(!userName.isEmpty()){
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"] = "userdisqonnected";
        disconnectedMessage["username"]=userName;
        broadcast(disconnectedMessage,nullptr);
        emit logMessage(userName +" disconnected ");
    }
    sender->deleteLater();
}
