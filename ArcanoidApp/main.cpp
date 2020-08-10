#include "ArcanoidApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  ArcanoidApp w;
  w.show();
  return a.exec();
}
