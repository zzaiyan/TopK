#ifndef CREATERANDNUMS_H
#define CREATERANDNUMS_H

#include <stdlib.h>
#include <time.h>
#include <QDebug>
#include <QString>
#include <chrono>
#include <cstring>
#include <iostream>
#include <vector>
#include "huffman.h"

constexpr int SUMNUM = 1000;  //随机数的数量
constexpr int MAXNUM = 999;   //随机数范围0---MAXNUM

constexpr double INTERNAL = 0.5;

//#pragma warning(disable : 4996)

using namespace std;
using namespace chrono;
class CreateRandNums {
 public:
  unsigned randseed;
  int* intMSG = nullptr;
  QString strMSG;
  QString hufMat;
  HuffmanTree* huf;
  //  //存储二进制编码的数组，数字用2进制存储，每个数字占10个字节
  //  char binaryCode[SUMNUM][10];

  CreateRandNums() {
    randseed = -1;
    intMSG = new int[SUMNUM];
    huf = new HuffmanTree;
  }

  //析构函数
  ~CreateRandNums() {
    delete[] intMSG;
    delete huf;
    //    cout << "CreateRandNums deleted" << endl;
  }

  void AddRandNums() {
    randseed++;
    srand(randseed);
    //        delete intMSG;
    for (int i = 0; i < SUMNUM; i++) {
      intMSG[i] =
          (rand() * RAND_MAX % MAXNUM + rand()) % MAXNUM + 0;  // 0~MAXNUM
    }
    //    for (int i = 0; i < 950; i++)
    //      intMSG[i] = 123;
  }

  //将随机数数组转换为字符串，同时转化为二进制
  void TransToHuffman() {
    strMSG.clear();
    for (int i = 0; i < SUMNUM; i++) {
      QString oneMSG = QString("%1").arg(
          intMSG[i], 3, 10, QLatin1Char('0'));  // 转换为3位十进制字符串
      strMSG.append(oneMSG);
      //            qDebug() << oneMSG;
    }
    int rawLen = strMSG.length();
    //    qDebug() << "rawTxt = " << strMSG;
    huf->importStr(strMSG);      // 建立哈夫曼树
    strMSG = huf->getSendBuf();  // 获取哈夫曼编码
    hufMat = huf->getMat();      // 获取哈夫曼矩阵
    qDebug() << QString("定长编码长度 = %1 , 哈夫曼编码长度 = %2")
                    .arg(rawLen * 8)
                    .arg(strMSG.length());
  }

  void TransToBinary() {
    strMSG.clear();
    for (int i = 0; i < SUMNUM; i++) {
      QString oneMSG = QString("%1").arg(
          intMSG[i], 24, 2, QLatin1Char('0'));  // 转换为24位二进制字符串
      strMSG.append(oneMSG);
      //            qDebug() << oneMSG;
    }
  }
};

#endif  // CREATERANDNUMS_H
