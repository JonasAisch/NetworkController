#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QMainWindow>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <QtNetwork>
#include <QTime>

namespace Ui {
class controller;
}

class controller : public QMainWindow
{
    Q_OBJECT

public:
    explicit controller(QWidget *parent = 0);
    ~controller();

private slots:
    void receivingUDP();
    void startUdpServer();
    void stopUdpServer();

    void sendPacket();

    void clearIncomingTable();
    void clearOutgoingTable();

    void on_pushButton_2_pressed();
    void on_pushButton_3_pressed();

    void startTcpServer();
    void acceptConnection();
    void getMessage();

private:
    Ui::controller *ui;
    void addtoTable(QString ipa, QString m, QString msg);
    void addtoTable2(QString ip, QString port, QString m, QString datagram);

    int rows = 0;
    int cols = 2;
    int rows2 = 0;
    int cols2 = 3;
    QUdpSocket *udpserver;
    QTcpServer *tcpserver;
    QUdpSocket *udpsender;
    QTcpSocket *tcpsender;
    QTcpSocket *currentclient;

    QTimer *timer;
};

#endif // CONTROLLER_H
