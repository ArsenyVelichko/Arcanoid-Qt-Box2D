#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Arcanoid.h"
#include <QGraphicsView>

namespace Ui {
  class Widget;
}

class Arcanoid : public QMainWindow
{
	Q_OBJECT

public:
	Arcanoid(QWidget *parent = Q_NULLPTR);
  ~Arcanoid();

private:
	Ui::ArcanoidClass* _ui;
  QGraphicsScene* _scene;
};
