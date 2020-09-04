#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ArcanoidApp.h"

class ArcanoidApp : public QMainWindow {
  Q_OBJECT

public:
  ArcanoidApp(QWidget* parent = Q_NULLPTR);

private:
  Ui::ArcanoidAppClass ui;
};
