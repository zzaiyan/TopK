#include "server.h"
#include <QDebug>
#include <QList>
#include <QNetworkInterface>
//#include "createrandnums.h"
#include "heap.h"
#include "ui_server.h"

Server::Server(QWidget* parent) : QWidget(parent), ui(new Ui::Server) {
  ui->setupUi(this);
  init();
  //  msgs = new CreateRandNums();  //生成处理随机数的类对象
}

//服务器端的析构函数
Server::~Server() {
  delete ui;
  //  if (msgs != NULL) {
  //    delete msgs;
  //  }
}
void Server::init() {
  // 初始化哈希表
  cnt = 0;
  hash = new HNode[1000];
  for (int i = 0; i < 1000; i++) {
    hash[i].num = i;
    hash[i].cnt = 0;
  }

  TcpServer = new QTcpServer(this);
  TcpSocket = new QTcpSocket(this);
  newListen();
  connect(TcpServer, SIGNAL(newConnection()), this,
          SLOT(mconnect()));  //建立连接
  connect(TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
          SLOT(displayError(QAbstractSocket::SocketError)));
}
//检查是否被占用
void Server::newListen() {
  if (!TcpServer->listen(QHostAddress::Any, 52996)) {
    qDebug() << TcpServer->errorString();
    close();
    return;
  }
}
//发送数据
void Server::on_pushButton_send_clicked() {
  QString str = ui->textEdit_write->toPlainText();
  TcpSocket->write(str.toUtf8().data());
  ui->textEdit_read->append(QString("[localhost]:%1").arg(str));
  ui->textEdit_write->clear();
}
//断开连接
void Server::on_pushButton_close_clicked() {
  TcpSocket->disconnectFromHost();
  TcpSocket->close();
}

void Server::mconnect() {
  qDebug() << "connected.";
  TcpSocket = TcpServer->nextPendingConnection();  //获取当前连接的socket
  connect(TcpSocket, SIGNAL(readyRead()), this, SLOT(RcvData()));
  QString ip = TcpSocket->peerAddress().toString();

  quint16 port = TcpSocket->peerPort();
  //    ui->textEdit_read->setText(QString::fromLocal8Bit("[%1:%2]:connected!").arg(ip).arg(port));
  ui->textEdit_read->setText(QString("[%1:%2]:connected.").arg(ip).arg(port));
}

void Server::displayError(QAbstractSocket::SocketError) {
  qDebug() << TcpSocket->errorString();
  TcpSocket->close();
}

/*
接收数据，请修改该函数以便实现利用类对象msgs对传输的二进制字符串解码，
请调用msgs->BinaryCodeToIntArray()或者msgs->HaffmanCodeToIntArray()，
将二进制字符串解码到msgs->intMSG整形数组中，并基于最大堆或者其他算法动态寻找最大的20个数
/ 出现次数最多的20个数
*/
void Server::RcvData() {
  QString rcvMsg = TcpSocket->readAll();
  //    ui->textEdit_read->setText(rcvMsg);
  QString ip = TcpSocket->peerAddress().toString();
  quint16 port = TcpSocket->peerPort();

  // Here：请修改以下代码，对从客户端接收到的二进制数据进行解码，然后从动态数据中找到最大的20个数
  // 出现次数最多的20个数并动态显示
  if (rcvMsg == QString("closed.")) {
    ui->textEdit_read->append(QString("[%1:%2]:closed.").arg(ip).arg(port));
    return;
  }

  int len = rcvMsg.size() - 1, t = 0;
  for (int i = 0; i < len; i++) {
    if (rcvMsg[i].isDigit()) {
      t = t * 10 + rcvMsg[i].toLatin1() - '0';
    }
    if (i + 1 == len || rcvMsg[i + 1].isSpace()) {
      hash[t].cnt++;
      t = 0;
    }
  }
  auto mhp = MaxHeap(HNode)(hash, 1000);

  int k = 20;
  QString outPut;
  outPut.append(QString("\nNo.%0:\tTotal: %100000\n").arg(cnt).arg(cnt));
  cnt++;
  while (k--) {
    auto top = mhp.pop();
    outPut.append(QString("Top %1  is  %2\ttimes = %3\n")
                      .arg(20 - k, 2)
                      .arg(top.num, 3)
                      .arg(top.cnt));
  }

  ui->textEdit_read->append(
      QString("[%1:%2]:%3").arg(ip).arg(port).arg(outPut));
}
