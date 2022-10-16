#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
//#include "createrandnums.h"
#include "huffman.h"

namespace Ui {
class Server;
}

extern int cmpTimes;

struct HNode {
  int num, cnt;  // value and times

  bool operator<(const HNode& a) const {
    cmpTimes++;
    return cnt < a.cnt || (cnt == a.cnt && num < a.num);
  }
};

class Server : public QWidget {
  Q_OBJECT
  HNode node;

 public:
  explicit Server(QWidget* parent = 0);
  ~Server();

 private slots:
  void init();

  void newListen();

  void mconnect();

  void RcvData();

  void on_pushButton_send_clicked();

  void on_pushButton_close_clicked();

  void displayError(QAbstractSocket::SocketError);

  void on_comboBox_currentIndexChanged(int index);

 private:
  Ui::Server* ui;
  QTcpServer* TcpServer;
  QTcpSocket* TcpSocket;
  HuffmanTree* huf;
  //  CreateRandNums* msgs = nullptr;
  int topkFrom = 0;

  //  void getTopK();

 public:
  unsigned long long cnt, total;
  HNode* hash;
  QString rcvMsg;
};

#endif  // SERVER_H
