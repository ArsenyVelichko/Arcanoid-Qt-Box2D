#include <QTextStream>
#include "GameMode.h"
#include "Surface.h"
#include "Block.h"
#include "Bonus.h"
#include "DeletionArea.h"

GameMode::GameMode(const QString& levelName, WorldScene* worldScene)
  : QObject(worldScene) {
  m_worldScene = worldScene;
  m_breakBlockCount = 0;

  createBall();

  QList<QPointF> boundVerts = {
    {-500.0, 300.0}, {-500.0, -300.0}, {500.0, -300.0}, {500.0, 300.0}
  };
  Surface* bounds = new Surface(boundVerts, worldScene);

  PaddleDef paddleDef;
  paddleDef.axis = {1.0, 0.0};
  paddleDef.pillarBody = bounds->getBody();
  paddleDef.origin = QPointF(0.0, 200.0);
  paddleDef.sizes = QSizeF(150.0, 10.0);
  paddleDef.speed = 600.0f;
  Paddle* paddle = new Paddle(paddleDef, worldScene);

  m_controller = new Controller<Paddle>(paddle);
  m_controller->bindKey(Qt::Key_A, &Paddle::switchLeft, &Paddle::switchRight, nullptr);
  m_controller->bindKey(Qt::Key_D, &Paddle::switchRight, &Paddle::switchLeft, nullptr);
  m_controller->bindKey(Qt::Key_Space, &Paddle::launchBall, nullptr, nullptr);
  worldScene->installEventFilter(m_controller);

  m_scorer = new Scorer;
  worldScene->addItem(m_scorer);
  m_scorer->setPos(-500.0, -300.0);

  m_hpIndicator = new HealthIndicator(3);
  float offset = m_hpIndicator->boundingRect().width();
  worldScene->addItem(m_hpIndicator);

  m_hpIndicator->setPos(500 - offset, -300);

  DeletionArea* delArea = new DeletionArea(QSizeF(1000.0, 60.0), worldScene);
  delArea->addAccepted("Ball");
  delArea->addAccepted("FallingBonus");
  delArea->setPos(0, 330);
  QObject::connect(delArea, &DeletionArea::deleted, this, &GameMode::catchBallDeletion);

  loadLevel(levelName);
}

void GameMode::createBall() {
  Ball* ball = new Ball(10.0f, QPointF(200.0f, -300.0f), m_worldScene);
  m_ballList.append(ball);
}

void GameMode::loadLevel(const QString& levelName) {
  QFile levelFile(levelName);
  if (levelFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream source(&levelFile);

    while (!source.atEnd()) {
      QString str = source.readLine();
      QStringList params = str.split(' ', Qt::SkipEmptyParts);
      createBlock(params);
    }
  }
  levelFile.close();
}

void GameMode::createBlock(const QStringList& params) {
  Block* block;

  char type = params[0][0].toLatin1();
  int durability = params[1].toInt();
  QSizeF sizes(params[2].toDouble(), params[3].toDouble());
  QPointF pos(params[4].toDouble(), params[5].toDouble());

  switch (type) {
  case 'D':
    block = new Block(durability, sizes, m_worldScene);
    break;

  case 'U':
    block = new Block(durability, sizes, m_worldScene);
    break;
  
  case 'A':
    block = new AccelBlock(50.0f, durability, sizes, m_worldScene);
    break;
  
  case 'B':
  {
    BonusData bonusData;
    bonusData.bonus = createBonus(params[6][0].toLatin1());
    bonusData.fallBonusSizes = QSizeF(20.0, 20.0);
    bonusData.fallBonusV = QPointF(0.0, 300.0);

    block = new BonusBlock(bonusData, durability, sizes, m_worldScene);
    break;
  }
  default:
    break;
  }

  if (durability != 0) {
    m_breakBlockCount++;
    QObject::connect(block, &Block::broken, this, &GameMode::countBlockDestruct);
    QObject::connect(block, &Block::damageTaken, m_scorer, &Scorer::increaseScore);
  }

  block->setPos(pos);
}

Bonus* GameMode::createBonus(char type) {
  Bonus* bonus;
  BonusManager* manager = m_worldScene->getBonusManager();

  switch (type) {
  case 'D':
    bonus = new DoubleBall(std::move(m_ballList), manager);
    break;

  case 'B':
    bonus = new AddBottom(QPointF(0.0, 250.0), QSizeF(1000.0, 30.0), manager);
    break;

  case 'A':
    bonus = new AccelerateBall(50.0f, std::move(m_ballList), manager);
    break;

  case 'S':
    bonus = new StickyPaddle(m_controller->getPawn(), 5.0f, manager);
    break;

  case 'R':
    bonus = new RandomDirect(0.004f, std::move(m_ballList), 5.0f, manager);
    break;

  case 'E':
    bonus = new ExpandPaddle(m_controller->getPawn(), 40.0f, manager);
    break;

  default:
    break;
  }
  return bonus;
}

void GameMode::countBlockDestruct() {
  m_breakBlockCount--;

  if (m_breakBlockCount == 0) {
    countWin();
  }
}

void GameMode::catchBallDeletion(GameObject* gameObj) {
  if (gameObj->getTag() == "Ball") {
    Ball* ball = static_cast<Ball*>(gameObj);
    m_ballList.removeOne(ball);

    if (m_ballList.isEmpty()) {

      m_hpIndicator->reduceHealth(1);
      if (m_hpIndicator->getHp() == 0) {
        countLose();
      } else {
        createBall();
      }
    }
  }
}

void GameMode::countWin() {
  QPixmap winPixmap(":/Textures/VictoryImage");
  auto pixmapItem = m_worldScene->addPixmap(winPixmap);
  pixmapItem->setPos(-winPixmap.width() / 2, -winPixmap.height() / 2);
  m_worldScene->stop();
}

void GameMode::countLose() {
  QPixmap losePixmap(":/Textures/DefeatImage");
  auto pixmapItem = m_worldScene->addPixmap(losePixmap);
  pixmapItem->setPos(-losePixmap.width() / 2, -losePixmap.height() / 2);
  m_worldScene->stop();
}

GameMode::~GameMode() {
  delete m_controller;
}