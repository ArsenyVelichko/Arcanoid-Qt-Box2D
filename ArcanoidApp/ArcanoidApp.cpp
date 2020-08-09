#include "ArcanoidApp.h"
#include <QGraphicsScene>

ArcanoidApp::ArcanoidApp(QWidget *parent)
  : QMainWindow(parent) {
  ui.setupUi(this);
  QGraphicsScene *scene = new QGraphicsScene;
  scene->addEllipse(-50, -50, 100, 100);
  ui.graphicsView->setScene(scene);
}
