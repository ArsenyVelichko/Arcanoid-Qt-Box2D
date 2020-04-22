#include "Arcanoid.h"
#include "GameBoard.h"
#include <fstream>

Arcanoid::Arcanoid(QWidget *parent)
	: QMainWindow(parent), _ui(new Ui::ArcanoidClass) {
	_ui->setupUi(this);
  setFixedSize(600, 800);

  QGraphicsView* view = new QGraphicsView;
  view->setFixedSize(600, 800);
  view->setRenderHint(QPainter::Antialiasing);
  view->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
  view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setCentralWidget(view);

  GameBoard* board = new GameBoard(QRectF(-300, -250, 600, 500), QRectF(-200, -250, 400, 500), 60);
  board->CreateCarriege(QVector2D(80, 10), -220, 5);
  board->CreateBall(10, 3, 5);
  int dims[2] = { 10, 10 };
  std::ifstream level("level1.txt");
  board->CreateBlocksGrid(QRectF(-200, 50, 400, 200), level);
  view->setScene(board);
  _scene = board;
}

Arcanoid::~Arcanoid() {
  delete _scene;
  delete _ui;
}