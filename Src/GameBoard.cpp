#include "GameBoard.h"
#include "GameObjects.h"
#include "CommonFuncs.h"
#include <QGraphicsProxyWidget>
#include <QPixmap>

GameBoard::GameBoard(const QRectF& sceneRect, const QRectF& gameAreaRect, int framesPerSecond) {
  _grid = nullptr;
  this->setSceneRect(sceneRect);
  _gameArea = new QRectF(gameAreaRect);

  _controller = new Controller();
  installEventFilter(_controller);

  _inGameTimer = new QTimer();
  
  QObject::connect(_inGameTimer, &QTimer::timeout, this, &GameBoard::advance);
  _inGameTimer->start(1000. / framesPerSecond);

  _scorer = new PlayerInterface();
  _scorer->display();

  QPointF pos = _gameArea->topLeft() - QPointF(75, 0);
  _scorer->createCounter(this, pos);

  pos += QPointF(0, 30);
  _scorer->createHearts(this, pos);

  _bonusH = _scorer->bonusHandler();
  srand(time(NULL));
}


bool GameBoard::CreateCarriege(const QVector2D& sizes, double heightOfLane, double speed) {
  if (_controller) {
    double halfX = sizes[0] / 2;
    double halfY = sizes[1] / 2;
    QRectF box(-halfX, heightOfLane - halfY, sizes[0], sizes[1]);

    Carriege* carr = new Carriege(box, speed);
    this->addItem(carr);
    carr->setPos(flipY(QPointF(0, heightOfLane)));
    _controller->addCarr(carr);

    return true;
  }
  return false;
}

bool GameBoard::CreateBall(double radius, double normalSpeed, double maxSpeed) {
  Carriege* carr = _controller->getCarr();

  if (_controller && carr) {
    static double s_radius = radius;
    static double s_normalSpeed = normalSpeed;
    static double s_maxSpeed = maxSpeed;

    QPointF center = carr->center() + QPointF(0, carr->height() / 2 + s_radius);
    Ball* ball = new Ball(center, QVector2D(0, 1), s_normalSpeed, s_maxSpeed, s_radius);

    this->addItem(ball);
    ball->setPos(flipY(ball->center()));
    _controller->addBall(ball);

    return true;
  }
  return false;
}

void GameBoard::CreateBlocksGrid(const QRectF& box, std::istream& source) {
  int dimensions[2];
  source >> dimensions[0];
  source >> dimensions[1];

  _grid = new BlocksGrid(box, dimensions);
  Block::setScorer(_scorer);

  for (int i = 0; i < dimensions[1]; i++) {
    for (int j = 0; j < dimensions[0]; j++) {

      BlockType type;
      char c;
      int hp = 1;
      source >> c;

      switch (c) {
      case 'D':
        type = BlockType::DEFAULT;
        source >> hp;
        break;

      case 'A':
        type = BlockType::ACCELERATING;
        source >> hp;
        break;

      case 'U':
        type = BlockType::UNBREAKABLE;
        break;

      default:
        _grid->addEmptyBlock(i, j);
        continue;
      }

      Block* block = _grid->CreateBlock(i, j, hp, type);
      this->addItem(block);
      block->setPos(flipY(block->center()));
    }
  }

  _scorer->createEndImages(this, flipY(_grid->center()));
}

QGraphicsItem* GameBoard::ClosestCollision(const Ball* ball) const {
  QList<QGraphicsItem*> obstructions = ball->collidingItems();
  double minDist = DBL_MAX;
  QGraphicsItem *closestItem = nullptr;

  for (QGraphicsItem *item : obstructions) {
    double dist = QVector2D(ball->center() - flipY(item->pos())).lengthSquared();

    if (dist < minDist) {
      minDist = dist;
      closestItem = item;
    }
  }

  return closestItem;
}

void GameBoard::advance() {
  QList<Ball*>& balls = _controller->getBalls();
  Carriege* carr = _controller->getCarr();
  HitRecord record;
  Block* brokenBlock;

  for (Ball* ball : balls) {
    ball->move();
    ball->setPos(flipY(ball->center()));
  }

  _grid->advance();
  _controller->advance(*_gameArea);
  _bonusH->advance(this, _gameArea->top(), carr);

  for (Ball* ball : balls) {
    if (ball->isMoving()) {

      Hittable* closest = dynamic_cast<Hittable*>(ClosestCollision(ball));
      if (closest) {
        closest->processBall(*ball);
      }

      if (isOutOfBounds(ball, record)) {
        ball->changeCenter(record.touchPoint + record.normal.toPointF() * ball->radius());
        ball->reflect(record.normal);
        ball->setPos(flipY(ball->center()));
      }  
    }
  }

  if (_grid->isEmpty()) {
    _scorer->countVictory();
    _inGameTimer->stop();
  }

  update();
}

static QVector2D correctDirection(const Ball* ball) {
  QVector2D currDirect = ball->direct();
  double maxY = currDirect.y() > 0 ? 1 : -1;

  double newY = currDirect.y() + 0.2 * pow(maxY - currDirect.y(), 3);
  double newX = sqrt((1 - newY * newY)) * sgn(currDirect.x());

  return QVector2D(newX, newY);
}

void GameBoard::restoreBall() {
  if (_controller->isEmpty()) {
    _scorer->reduceHeart(this);

    if (!_scorer->isHeartsLeft()) {
      _scorer->countLose();
      _inGameTimer->stop();

    }
    else {
      CreateBall(0, 0, 0);
    }
  }
}

bool GameBoard::isOutOfBounds(Ball* ball, HitRecord& record)  {
  QPointF ballCenter = ball->center();

  double halfWidth = _gameArea->width() / 2;
  double halfHeight = _gameArea->height() / 2;
  bool isHit = false;

  if (ballCenter.x() + ball->radius() > halfWidth) {
    isHit = true;
    record.normal = QVector2D(-1, 0);
    record.touchPoint = QPointF(halfWidth, ballCenter.y());

    ball->changeDirect(correctDirection(ball));

  } else if (ballCenter.x() - ball->radius() < -halfWidth) {
    isHit = true;
    record.normal = QVector2D(1, 0);
    record.touchPoint = QPointF(-halfWidth, ballCenter.y());

    ball->changeDirect(correctDirection(ball));

  } else if (ballCenter.y() + ball->radius() > halfHeight) {
    isHit = true;
    record.normal = QVector2D(0, -1);
    record.touchPoint = QPointF(ballCenter.x(), halfHeight);

  } else if (ballCenter.y() - ball->radius() < -halfHeight) {
    _controller->removeBall(ball);
    removeItem(ball);
    delete ball;

    restoreBall();
  }

  return isHit;
}

GameBoard::~GameBoard() {
  delete _grid;
  delete _gameArea;
  delete _inGameTimer;
  delete _controller;
  delete _scorer;
}