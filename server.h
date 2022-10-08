#ifndef WIDGET_H
#define WIDGET_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
#include "createrandnums.h"

namespace Ui {
class Server;
}

// struct HNode {
//   long long num, cnt;  // value and times
//   bool operator<(const HNode& a) const {
//     return cnt < a.cnt || (cnt == a.cnt && num < a.num);
//   }
// };

class Server : public QWidget {
  Q_OBJECT

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

 private:
  Ui::Server* ui;
  QTcpServer* TcpServer;
  QTcpSocket* TcpSocket;
  CreateRandNums* msgs = nullptr;

 public:
  int cnt;
  HNode* hash;
  QString rcvMsg;
};

#endif  // WIDGET_H
