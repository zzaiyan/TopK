#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <bits/stdc++.h>
#include <QDebug>
#include <QString>
#include <iostream>
#include <string>

constexpr int MAXBIT = 10;
constexpr int MAXVALUE = 10000;
constexpr int MAXLEAF = 10;
constexpr int MAXNODE = MAXLEAF * 2 - 1;

using namespace std;

struct HCodeType {
  //  QString bit;
  int bit[MAXLEAF];
  int start;
}; /* 编码结构体 */

struct HNodeType {
  int weight, parent, lChild, rChild;
  char value;
}; /* 结点结构体 */

class HuffmanTree {
  HNodeType HuffNode[MAXNODE];
  HCodeType HuffCode[MAXLEAF];

  int n = 10;  // 字典大小

  void init() {
    for (int i = 0; i < MAXNODE; i++) {
      HuffNode[i].weight = 0;  //权值
      HuffNode[i].parent = -1;
      HuffNode[i].lChild = -1;
      HuffNode[i].rChild = -1;
      HuffNode[i].value = '@';  //实际值，可根据情况替换为字母
    }
  }

 public:
  QString Dict[MAXLEAF];
  QString txtBuf, sendBuf;

  HuffmanTree() { init(); }

  void importStr(const QString& str) {  // 通过字符串建立哈夫曼树
    init();
    txtBuf = str;
    int i, j, m1, m2, x1, x2;
    // 首先统计数字出现的次数
    int charCnt[10]{0};
    for (auto ch : str) {
      charCnt[ch.toLatin1() - '0']++;
    }
    for (int i = 0; i < 10; i++) {
      HuffNode[i].value = i + '0';
      HuffNode[i].weight = charCnt[i];
    }
    for (i = 0; i < n - 1; i++) {
      m1 = m2 = MAXVALUE; /* m1、m2中存放两个无父结点且结点权值最小的两个结点 */
      x1 = x2 = 0;
      /* 找出所有结点中权值最小、无父结点的两个结点，并合并之为一颗二叉树 */
      for (j = 0; j < n + i; j++) {
        if (HuffNode[j].weight < m1 && HuffNode[j].parent == -1) {
          m2 = m1;
          x2 = x1;
          m1 = HuffNode[j].weight;
          x1 = j;
        } else if (HuffNode[j].weight < m2 && HuffNode[j].parent == -1) {
          m2 = HuffNode[j].weight;
          x2 = j;
        }
      } /* end for */
      /* 设置找到的两个子结点 x1、x2 的父结点信息 */
      HuffNode[x1].parent = n + i;
      HuffNode[x2].parent = n + i;
      HuffNode[n + i].weight = HuffNode[x1].weight + HuffNode[x2].weight;
      HuffNode[n + i].lChild = x1;
      HuffNode[n + i].rChild = x2;
    }
    calcDict();  // 计算字典
  }

  void calcDict() {  //计算字典
    string ret;
    HCodeType cd;
    int i, j, c, p;
    for (i = 0; i < n; i++) {
      cd.start = n - 1;
      c = i;
      p = HuffNode[c].parent;
      while (p != -1) /* 父结点存在 */
      {
        if (HuffNode[p].lChild == c)
          cd.bit[cd.start] = 0;
        else
          cd.bit[cd.start] = 1;
        cd.start--; /* 求编码的低一位 */
        c = p;
        p = HuffNode[c].parent; /* 设置下一循环条件 */
      }                         /* end while */

      /* 保存求出的每个叶结点的哈夫曼编码和编码的起始位 */
      for (j = cd.start + 1; j < n; j++) {
        HuffCode[i].bit[j] = cd.bit[j];
      }
      HuffCode[i].start = cd.start;
    } /* end for */
    for (int i = 0; i < 10; i++) {
      QString buf;
      for (int j = HuffCode[i].start + 1; j < 10; j++)
        buf.append(QString::number(HuffCode[i].bit[j]));
      //      qDebug() << buf;
      Dict[i] = buf;
    }
    //    for (int i = 0; i < MAXLEAF; i++) {
    //      qDebug() << i << " = " << Dict[i];
    //    }
  }

  QString getSendBuf() {
    sendBuf.clear();
    for (auto ch : txtBuf) {
      sendBuf.append(Dict[ch.toLatin1() - '0']);
    }
    //    qDebug() << "sendBuf = " << sendBuf;
    return sendBuf;
  }

  QString getMat() {  // 返回矩阵字符串，用于接收端解码
    QString ret;
    for (int i = 0; i < MAXNODE; i++) {
      QString buf;
      buf.append(
          QString("%1").arg(HuffNode[i].weight, 5, 10, QLatin1Char('0')));
      if (HuffNode[i].parent == -1)
        buf.append("99");
      else
        buf.append(
            QString("%1").arg(HuffNode[i].parent, 2, 10, QLatin1Char('0')));

      if (HuffNode[i].lChild == -1)
        buf.append("99");
      else
        buf.append(
            QString("%1").arg(HuffNode[i].lChild, 2, 10, QLatin1Char('0')));
      if (HuffNode[i].rChild == -1)
        buf.append("99");
      else
        buf.append(
            QString("%1").arg(HuffNode[i].rChild, 2, 10, QLatin1Char('0')));

      buf.append(QString(HuffNode[i].value));

      //      auto a = QString("%1%2%3%4%5 ")
      //                   .arg(HuffNode[i].weight, 5, 10, QLatin1Char('0'))
      //                   .arg(HuffNode[i].parent, 2, 10, QLatin1Char('0'))
      //                   .arg(HuffNode[i].lChild, 2, 10, QLatin1Char('0'))
      //                   .arg(HuffNode[i].rChild, 2, 10, QLatin1Char('0'))
      //                   .arg(HuffNode[i].value);
      ret.append(buf);
    }
    //    qDebug() << "Mat.size = " << ret.size();
    return ret;
  }

  void importMat(const QString& mat) {  // 导入收到的矩阵字符串，用于解码
                                        //    qDebug() << "importMat ing!";
    for (int i = 0; i < MAXNODE; i++) {
      int s = i * 12;
      QStringRef temp;
      temp = mat.midRef(s + 0, 5);  // 0-4
      HuffNode[i].weight = temp.toInt();
      temp = mat.midRef(s + 5, 2);  // 5-6
      if (temp != "99")
        HuffNode[i].parent = temp.toInt();
      else
        HuffNode[i].parent = -1;
      temp = mat.midRef(s + 7, 2);  // 7-8
      if (temp != "99")
        HuffNode[i].lChild = temp.toInt();
      else
        HuffNode[i].lChild = -1;
      temp = mat.midRef(s + 9, 2);  // 9-10
      if (temp != "99")
        HuffNode[i].rChild = temp.toInt();
      else
        HuffNode[i].rChild = -1;

      HuffNode[i].value = mat[s + 11].toLatin1();

      //      qDebug() << "temp = " << temp;
    }
  }

  QString decoding(const QString& str) {  // 将输入的二进制字符串解码，返回结果
    QString ret;
    int i, tmp = 0;
    char num[16384];
    for (i = 0; i < str.size(); i++) {
      if (str[i] == '0')
        num[i] = 0;
      else
        num[i] = 1;
    }
    i = 0;
    while (i < str.size()) {
      tmp = MAXNODE - 1;
      while ((HuffNode[tmp].lChild != -1) && (HuffNode[tmp].rChild != -1)) {
        if (num[i] == 0) {
          tmp = HuffNode[tmp].lChild;
        } else
          tmp = HuffNode[tmp].rChild;
        i++;
      }
      //      printf("%c", HuffNode[tmp].value);
      ret.push_back(QString(HuffNode[tmp].value));
    }
    //    qDebug() << ret;
    return ret;
  }
};

#endif  // HUFFMAN_H
