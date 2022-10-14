﻿#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QWidget>
#include "createrandnums.h"
#include "huffman.h"

namespace Ui {
class Client;
}

class Client : public QWidget {
  Q_OBJECT

 public:
  explicit Client(QWidget* parent = 0);
  ~Client();

 private slots:

  void displayError(QAbstractSocket::SocketError);

  void on_pushButton_connect_clicked();

  void recivedata();

  void on_pushButton_send_clicked();

  void on_pushButton_stop_clicked();

  void on_pushButton_close_clicked();

  void on_pushButton_start_clicked();

  void on_comboBox_currentIndexChanged(int index);

 public slots:
  void onTimeOut();

 private:
  Ui::Client* ui;
  QTcpSocket* socket;
  QTimer* tim;
  //  HuffmanTree* huf;
  CreateRandNums* msgs = nullptr;

  int codingMode = 0;

  void send_data();
};

#endif  // CLIENT_H
