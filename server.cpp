#include "server.h"
#include <QDebug>
#include <QList>
#include <QNetworkInterface>
#include "heap.h"
#include "ui_server.h"

#define PORT 52996

Server::Server(QWidget* parent) : QWidget(parent), ui(new Ui::Server) {
  ui->setupUi(this);
  init();
  //  msgs = new CreateRandNums();  //生成处理随机数的类对象
}

//服务器端的析构函数
Server::~Server() {
  delete ui;
  delete[] hash;
  delete huf;
}
void Server::init() {
  // 初始化哈希表
  cnt = 0;
  hash = new HNode[1000];
  huf = new HuffmanTree;
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
  if (!TcpServer->listen(QHostAddress::Any, PORT)) {
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
  rcvMsg = TcpSocket->readAll();
  //    ui->textEdit_read->setText(rcvMsg);
  QString ip = TcpSocket->peerAddress().toString();
  quint16 port = TcpSocket->peerPort();

  // Here：请修改以下代码，对从客户端接收到的二进制数据进行解码，然后从动态数据中找到最大的20个数
  // 出现次数最多的20个数并动态显示
  if (rcvMsg == QString("closed.")) {
    ui->textEdit_read->append(QString("[%1:%2]:closed.").arg(ip).arg(port));
    return;
  }
  if (rcvMsg.front() == 'B') {  // 普通二进制包
    int len = rcvMsg.size() / 10;
    auto str = rcvMsg.toLatin1().data();
    //    qDebug() << Qt::endl;
    for (int i = 0; i < len; i++) {
      int t = (str[i * 10 + 1] - '0') * 1 + (str[i * 10 + 2] - '0') * 2 +
              (str[i * 10 + 3] - '0') * 4 + (str[i * 10 + 4] - '0') * 8 +
              (str[i * 10 + 5] - '0') * 16 + (str[i * 10 + 6] - '0') * 32 +
              (str[i * 10 + 7] - '0') * 64 + (str[i * 10 + 8] - '0') * 128 +
              (str[i * 10 + 9] - '0') * 256 + (str[i * 10 + 10] - '0') * 512;
      hash[t].cnt++;
    }

  } else if (rcvMsg.front() == 'M') {  // 普通文本信息
    ui->textEdit_read->append(
        QString("[%1:%2]:%3").arg(ip).arg(port).arg(rcvMsg));
    return;

  } else if (rcvMsg.front() == 'H') {  // 哈夫曼数据包
    auto mat = rcvMsg.mid(1, 228);
    huf->importMat(mat);
    ui->textEdit_read->append(
        QString("[%1:%2]:%3").arg(ip).arg(port).arg("Matrix get."));

    auto hufTxt = rcvMsg.right(rcvMsg.length() - 229);
    //    qDebug() << "hufTxt = " << hufTxt;
    auto decodeTxt = huf->decoding(hufTxt);
    //    qDebug() << "decodeTxt = " << decodeTxt;
    int len = decodeTxt.length();
    auto str = decodeTxt.toLatin1().data();
    //    qDebug() << Qt::endl;
    for (int i = 0; i < len; i += 3) {
      int t =
          (str[i] - '0') * 100 + (str[i + 1] - '0') * 10 + (str[i + 2] - '0');
      hash[t].cnt++;
      if (t < 0)
        qDebug() << "Trans Num = " << t;
    }
  }

  // 建堆，统计Top20
  auto mhp = MaxHeap(HNode)(hash, 1000);

  int k = 20;
  QString outPut;
  outPut.append(QString("\nNo.%1:\tTotal: %20000\n").arg(cnt).arg(cnt));
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
