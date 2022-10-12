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

constexpr int SUMNUM = 20;    //随机数的数量
constexpr int MAXNUM = 1000;  //随机数范围0---MAXNUM

constexpr double INTERNAL = 10 * 0.5;

//#pragma warning(disable : 4996)

using namespace std;
using namespace chrono;
class CreateRandNums {
 public:
  unsigned randseed;
  int* intMSG = nullptr;
  QString strMSG = "";
  //  //存储二进制编码的数组，数字用2进制存储，每个数字占10个字节
  //  char binaryCode[SUMNUM][10];

  CreateRandNums() {
    randseed = -1;
    intMSG = new int[SUMNUM];
  }

  //析构函数
  ~CreateRandNums() {
    if (intMSG) {
      delete intMSG;
    }
    cout << "Object is being deleted" << endl;
  }

  void AddRandNums() {
    randseed++;
    srand(randseed);
    //        delete intMSG;
    for (int i = 0; i < SUMNUM; i++) {
      intMSG[i] =
          (rand() * RAND_MAX % MAXNUM + rand()) % MAXNUM + 0;  // 0~MAXNUM
    }
  }

  //将随机数数组转换为字符串，同时转化为二进制
  void Transform() {
    strMSG.clear();
    for (int i = 0; i < SUMNUM; i++) {
      QString oneMSG = QString("%1").arg(
          intMSG[i], 3, 10, QLatin1Char('0'));  // 转换为3位十进制字符串
      strMSG.append(oneMSG);
      //      qDebug() << oneMSG;
    }
  }

  // Your Code
  // Here：将普通的未压缩二进制(字符串/数组)解码到十进制随机数数组，用于服务器端解码
  void BinaryCodeToIntArray() {}

  // Your Code
  // Here：将随机数数组编码为haffman算法编码后的二进制数组，务必同步传输字典，用于在客户端编码
  void ToHaffmanCode() {}

  // ----------------------------------
  // Your Code
  // Here：将haffman算法编码后的二进制(字符串/数组)解码到十进制随机数数组，用于服务器端解码
  void HaffmanCodeToIntArray() {}

  // Your Code Here：新增将二进制数组binaryCode转为字符串的函数
  void TransformBinaryCode() {}
};

#endif  // CREATERANDNUMS_H
