#include "client.h"
#include <QDebug>
#include <QHostAddress>
#include <QTimer>
#include "createrandnums.h"
#include "ui_client.h"

Client::Client(QWidget* parent) : QWidget(parent), ui(new Ui::Client) {
  ui->setupUi(this);
  socket = new QTcpSocket(this);
  connect(socket, SIGNAL(readyRead()), this, SLOT(recivedata()));
  connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
          SLOT(displayError(QAbstractSocket::SocketError)));

  //设置定时器，每隔30s自动调用onTimeOut函数发送1万个随机数去服务器端
  tim = new QTimer();
  tim->setInterval(1000 * INTERNAL);
  connect(tim, SIGNAL(timeout()), this, SLOT(onTimeOut()));
  msgs = new CreateRandNums();  //生成处理随机数的类对象
  huf = new HuffmanTree;

  ui->pushButton_connect->setEnabled(1);
  ui->pushButton_send->setEnabled(0);
}
//析构函数
Client::~Client() {
  delete ui;
  if (msgs != NULL) {
    delete msgs;
  }
}

//建立连接按钮事件
void Client::on_pushButton_connect_clicked() {
  ui->pushButton_connect->setEnabled(0);
  socket->abort();
  QString ip = ui->lineEdit_ip->text();
  quint16 port = ui->lineEdit_port->text().toInt();
  qDebug() << ip << port;
  socket->connectToHost(QHostAddress(ip), port);
  ui->pushButton_close->setEnabled(1);
  ui->pushButton_send->setEnabled(1);
}

void Client::displayError(QAbstractSocket::SocketError) {
  qDebug() << socket->errorString();
  socket->close();
}

//接收数据
void Client::recivedata() {
  QString datas = socket->readAll();
  //    ui->textEdit_read->setText(datas);
  QString ip = socket->peerAddress().toString();
  quint16 port = socket->peerPort();
  ui->textEdit_read->append(QString("[%1:%2]:%3").arg(ip).arg(port).arg(datas));
}

void Client::send_data() {
  //初始传输1万个随机数到服务器，此处传输的是字符串，供参考，请修改为传输二进制字符串
  msgs->AddRandNums();  //每次生成1万个随机数，存储在类中的变量intMSG中
  msgs->Transform();  //整形数组intMSG转为字符串，存储在类中的变量strMSG中

  // Your Code Here：将随机数数组编码为普通的未压缩二进制数组 or
  // 哈夫曼编码的数组（对比），用于在客户端编码
  // msgs->ToBinaryCode();		//整形数组转为未压缩二进制数组
  // msgs->ToHaffmanCode(); 		//整形数组转为Haffman编码后的二进制数组
  // msgs->TransformBinaryCode();
  // 二进制数组转为类中的变量strMSG(字符串)以便进行数据传输

  QString txt = msgs->strMSG, sendBuf;
  huf->importStr(txt);  // 导入字符串
  qDebug() << "txt = " << txt;
  huf->calcDict();  //计算字典
  for (int i = 0; i < txt.size(); i++) {
    sendBuf.append(huf->Dict[txt[i].toLatin1() - '0']);
  }
  qDebug() << "sendBuf = " << sendBuf;
  QString mat = huf->getMat();
  qDebug() << "mat = " << mat;
  //  socket->write(txt.toUtf8().data());
  ui->textEdit_read->append(
      QString("[local]: Text sent: size=%1").arg(txt.size()));
  qDebug() << "send size = " << txt.size();
  huf->importMat(mat);
}

//发送数据
void Client::on_pushButton_send_clicked() {
  QString txt = ui->textEdit_write->toPlainText();
  socket->write(txt.toUtf8().data());
  qDebug() << txt.toUtf8().data() << "\tsize=" << txt.toUtf8().size();
  ui->textEdit_read->append(QString("[localclient]:%1").arg(txt));
  ui->textEdit_write->clear();
}

//定时器每隔30s自动调用本函数发送1万个随机数去服务器端
void Client::onTimeOut() {
  send_data();
}

//停止定时器
void Client::on_pushButton_stop_clicked() {
  tim->stop();
}

void Client::on_pushButton_close_clicked() {
  ui->pushButton_close->setEnabled(0);
  ui->pushButton_send->setEnabled(0);
  tim->stop();
  msgs->strMSG.clear();
  socket->write(QString("closed.").toUtf8().data());
  socket->close();
  ui->pushButton_connect->setEnabled(1);
}

void Client::on_pushButton_start_clicked() {
  send_data();
  tim->start();
}
