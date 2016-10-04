#include "controller.h"
#include "ui_controller.h"

const int port = 1337;
bool udprunning = false;
bool tcprunning = false;

controller::controller(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::controller)
{
    ui->setupUi(this);

    this->setWindowTitle("NetworkController 🎚");
    this->setFixedSize(size());

    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(receivingUDP()));

    connect(ui->pushButton_4, SIGNAL(clicked(bool)), this, SLOT(clearOutgoingTable()));
    connect(ui->pushButton_5, SIGNAL(clicked(bool)), this, SLOT(clearIncomingTable()));
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(sendPacket()));
}

controller::~controller()
{
    delete ui;
}

void controller::receivingUDP() {
    while(udpserver->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpserver->pendingDatagramSize());
        udpserver->readDatagram(datagram.data(), datagram.size());
        addtoTable("1.1.1", "UDP", datagram.data());
    }
}

void controller::addtoTable(QString ipa, QString m, QString buffer){
    ui->tableWidget->setRowCount(rows + 1);

    QTableWidgetItem *msg = new QTableWidgetItem(buffer);
    ui->tableWidget->setItem(0, cols, msg);
    msg->setFlags(msg->flags() ^ Qt::ItemIsEditable);

    QTableWidgetItem *ip = new QTableWidgetItem(ipa);
    ui->tableWidget->setItem(rows, 0, ip);
    ip->setFlags(ip->flags() ^ Qt::ItemIsEditable);


    QTableWidgetItem *method = new QTableWidgetItem(m);
    ui->tableWidget->setItem(0, cols - 1, method);
    method->setFlags(method->flags() ^ Qt::ItemIsEditable);

    cols += 3;
    rows += 1;
}

void controller::startUdpServer() {
    udpserver = new QUdpSocket(this);
    udpserver->bind(port, QUdpSocket::ShareAddress);
    timer->start(1000);
}

void controller::stopUdpServer() {
    udpserver->close();
    delete udpserver;
    timer->stop();
}

void controller::on_pushButton_2_pressed()
{
    if (tcprunning == false) {
        ui->pushButton_2->setCheckable(true);
        tcprunning = true;
        ui->pushButton_2->setStyleSheet("color: rgb(255,255,255)");
        startTcpServer();
        return;
    }
    if (tcprunning == true) {
        ui->pushButton_2->setCheckable(false);
        tcprunning = false;
        ui->pushButton_2->setStyleSheet("color: rgb(0,0,0)");
        return;
    }
}

void controller::on_pushButton_3_pressed()
{
    if (udprunning == false) {
        ui->pushButton_3->setCheckable(true);
        udprunning = true;
        ui->pushButton_3->setStyleSheet("color: rgb(255,255,255)");

        startUdpServer();

        return;
    }
    if (udprunning == true) {
        ui->pushButton_3->setCheckable(false);
        udprunning = false;
        ui->pushButton_3->setStyleSheet("color: rgb(0,0,0)");

        stopUdpServer();

        return;
    }
}

void controller::clearIncomingTable() {
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(3);

    rows = 0;
    cols = 2;
}

void controller::clearOutgoingTable() {
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->setColumnCount(4);

    rows2 = 0;
    cols2 = 3;
}

void controller::addtoTable2(QString ip, QString port, QString m, QString datagram) {
    ui->tableWidget_2->setRowCount(rows2 + 1);

    QTableWidgetItem *msg = new QTableWidgetItem(datagram);
    ui->tableWidget_2->setItem(0, cols2, msg);
    msg->setFlags(msg->flags() ^ Qt::ItemIsEditable);

    QTableWidgetItem *ipa = new QTableWidgetItem(ip);
    ui->tableWidget_2->setItem(rows2, 0, ipa);
    ipa->setFlags(ipa->flags() ^ Qt::ItemIsEditable);


    QTableWidgetItem *method = new QTableWidgetItem(m);
    ui->tableWidget_2->setItem(0, cols2 - 1, method);
    method->setFlags(method->flags() ^ Qt::ItemIsEditable);


    QTableWidgetItem *p = new QTableWidgetItem(port);
    ui->tableWidget_2->setItem(0, cols2 - 2, p);
    p->setFlags(p->flags() ^ Qt::ItemIsEditable);

    cols2 += 4;
    rows2 += 1;
}

void controller::sendPacket() {
    if (ui->lineEdit_2->text().size() == 0 || ui->lineEdit_3->text().size() == 0) {
        return;
    }
    if(ui->comboBox->currentIndex() == 0) {
        tcpsender = new QTcpSocket(this);
        QHostAddress addr = QHostAddress(ui->lineEdit_3->text());
        QByteArray dmg;
        dmg.append(ui->textEdit->toPlainText());

        tcpsender->connectToHost(addr, ui->lineEdit_2->text().toInt());
        tcpsender->write(dmg);

        addtoTable2(ui->lineEdit_3->text(), ui->lineEdit_2->text(), "TCP", ui->textEdit->toPlainText());
    }
    if (ui->comboBox->currentIndex() == 1) {
        // UDP
        udpsender = new QUdpSocket(this);
        QByteArray dmg;
        QString addr = ui->lineEdit_3->text();
        dmg.append(ui->textEdit->toPlainText());
        QHostAddress desip = QHostAddress(ui->lineEdit_3->text());

        udpsender->writeDatagram(dmg.data(), dmg.size(), desip, ui->lineEdit_2->text().toInt());

        udpsender->close();

        delete udpsender;

        addtoTable2(ui->lineEdit_3->text(), ui->lineEdit_2->text(), "UDP", ui->textEdit->toPlainText());
    }
}

void controller::startTcpServer() {
    connect(tcpserver, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    tcpserver->listen(QHostAddress::Any, port);
}

void controller::acceptConnection() {
    currentclient = tcpserver->nextPendingConnection();
    connect(currentclient, SIGNAL(readyRead()), this, SLOT(getMessage()));
}

void controller::getMessage() {
    char buffer[1024];

    currentclient->read(buffer, currentclient->bytesAvailable());

    addtoTable(QString::number(currentclient->IPv4Protocol), "TCP", buffer);

    currentclient->close();
}
