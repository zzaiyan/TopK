#include "server.h"
#include <QDebug>
#include <QList>
#include <QNetworkInterface>
#include "heap.h"
#include "ui_server.h"

#define PORT 52996

int cmpTimes;

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
  delete TcpServer;
  delete TcpSocket;
}

void Server::init() {
  // 初始化哈希表
  cnt = 0, total = 0;
  hash = new HNode[1000];
  huf = new HuffmanTree;
  for (int i = 0; i < 1000; i++) {
    hash[i].num = i;
    hash[i].cnt = 0;
  }

  //  rcvMsg.reserve(120005);

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
  ui->textEdit_read->append(QString("[localhost]: %1").arg(str));
  ui->lineEdit->setText("发送普通文本信息……");
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

void Server::RcvData() {
  rcvMsg = TcpSocket->readAll();
  //  qDebug() << rcvMsg.size();
  QString ip = TcpSocket->peerAddress().toString();
  quint16 port = TcpSocket->peerPort();

  if (rcvMsg == QString("closed.")) {
    ui->textEdit_read->append(QString("[%1:%2]:closed.").arg(ip).arg(port));
    return;
  }
  auto start = chrono::high_resolution_clock::now();  // 统计时间
  // 处理收到的数据包
  if (rcvMsg.front() == 'B') {  // 普通二进制包
    ui->lineEdit->setText("接收定长编码……");
    int len = rcvMsg.length() / 8;
    //    qDebug() << Qt::endl;
    for (int i = 0; i < len;) {
      int t = 0;
      for (int j = 0; j < 3; i++, j++) {
        t *= 10;
        t += (rcvMsg[i * 8 + 8].toLatin1() - '0') * 1 +
             (rcvMsg[i * 8 + 7].toLatin1() - '0') * 2 +
             (rcvMsg[i * 8 + 6].toLatin1() - '0') * 4 +
             (rcvMsg[i * 8 + 5].toLatin1() - '0') * 8 +
             (rcvMsg[i * 8 + 4].toLatin1() - '0') * 16 +
             (rcvMsg[i * 8 + 3].toLatin1() - '0') * 32 +
             (rcvMsg[i * 8 + 2].toLatin1() - '0') * 64 +
             (rcvMsg[i * 8 + 1].toLatin1() - '0') * 128 - '0';
      }
      //      qDebug() << "t = " << t;
      total++;
      hash[t].cnt++;
    }
  } else if (rcvMsg.front() == 'M') {  // 普通文本信息
    ui->lineEdit->setText("接收普通文本信息……");
    ui->textEdit_read->append(
        QString("[%1:%2]: %3").arg(ip).arg(port).arg(rcvMsg.remove(0, 1)));
    return;

  } else if (rcvMsg.front() == 'H') {  // 哈夫曼数据包
    ui->lineEdit->setText("接收哈夫曼编码……");
    auto mat = rcvMsg.mid(1, 228);
    huf->importMat(mat);
    ui->textEdit_read->append(
        QString("[%1:%2]: %3").arg(ip).arg(port).arg("字典已接收。"));

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
      total++;
      hash[t].cnt++;
    }
  }

  // 统计出现次数前20的元素
  cmpTimes = 0;  //次数置零

  QString outPut;  // 结果输出字符串
  cnt++;           // 记录当前是第几波数据
  outPut.append(QString("\nNo.%1:\tTotal: %2\n").arg(cnt).arg(total));
  // 算法四选一
  if (topkFrom == 0) {                      // 建堆，统计Top20
    auto mhp = MaxHeap(HNode)(hash, 1000);  // 声明一个最大堆
    //    Heap<HNode, HNode> mhp(hash, 1000);

    for (int k = 0; k < 20; k++) {  // 依次从堆顶取出20个元素
      auto top = mhp.pop();
      outPut.append(QString("Top %1  is  %2\ttimes = %3\n")
                        .arg(k + 1, 2)
                        .arg(top.num, 3)
                        .arg(top.cnt));
    }
  } else if (topkFrom == 1) {  // 红黑树统计
    // RB-Tree
    auto rbtree = set<HNode>();
    auto p = rbtree.begin();
    for (int i = 0; i < 1000; i++) {
      rbtree.insert(hash[i]);
      p++;
    }
    p--;
    for (int k = 0; k < 20; k++, p--) {
      outPut.append(QString("Top %1  is  %2\ttimes = %3\n")
                        .arg(k + 1, 2)
                        .arg((*p).num, 3)
                        .arg((*p).cnt));
    }

  } else if (topkFrom == 2) {  // 快速排序统计Top20
    Vector<HNode> vec(hash, 1000);
    sort(vec.begin(), vec.end());  // 调用 STL 排序算法

    for (int k = 0; k < 20; k++) {
      outPut.append(QString("Top %1  is  %2\ttimes = %3\n")
                        .arg(k + 1, 2)
                        .arg(vec[999 - k].num, 3)
                        .arg(vec[999 - k].cnt));
    }
  } else {
    Vector<HNode> vec(hash, 1000);
    for (int i = 0; i < 20; i++) {
      int maxJ = i;
      for (int j = i; j < 1000; j++) {  //寻找第 i+1 大的元素
        if ((vec[maxJ] < vec[j]))
          maxJ = j;
      }
      swap(vec[i], vec[maxJ]);
    }
    for (int k = 0; k < 20; k++) {
      outPut.append(QString("Top %1  is  %2\ttimes = %3\n")
                        .arg(k + 1, 2)
                        .arg(vec[k].num, 3)
                        .arg(vec[k].cnt));
    }
  }
  auto end = chrono::high_resolution_clock::now();  // 统计时间
  auto runTime = (end - start).count();
  //  qDebug() << cmpTimes;
  ui->cntLabel->setText(QString("比较次数：%1 次.").arg(cmpTimes));
  ui->timeLabel->setText(QString("耗时：%1 ms.").arg(runTime / 1e6));
  ui->textEdit_read->append(
      QString("[%1:%2]: %3").arg(ip).arg(port).arg(outPut));
}

void Server::on_comboBox_currentIndexChanged(int index) {
  topkFrom = index;
}
