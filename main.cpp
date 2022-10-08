#include <QApplication>
#include <QScreen>
#include "client.h"
#include "server.h"
int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  Server s;
  Client c;
  s.show();
  c.show();
  {
    int screen_X = QGuiApplication::primaryScreen()->geometry().width();
    int screen_Y = QGuiApplication::primaryScreen()->geometry().height();
    int pos_X = (screen_X - (c.width() + s.width() + 20)) / 2;
    int pos_Y = (screen_Y - c.height()) / 2;
    c.move(pos_X, pos_Y);
    s.move(pos_X + c.width() + 20, pos_Y);
  }
  return a.exec();
}
