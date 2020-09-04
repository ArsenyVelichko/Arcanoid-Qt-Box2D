#include "ArcanoidApp.h"
#include "WorldScene.h"
#include "GameMode.h"

ArcanoidApp::ArcanoidApp(QWidget* parent)
  : QMainWindow(parent) {
  ui.setupUi(this);

  QRectF sceneRect(-640, -360, 1280, 720);
  WorldScene* scene = new WorldScene(1.0f / 120.0f, sceneRect, ui.graphicsView);
  ui.graphicsView->setScene(scene);

  GameMode* gm = new GameMode(":/Levels/Level1", scene);

  scene->start();
}
