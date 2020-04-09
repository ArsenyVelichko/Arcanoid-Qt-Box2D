#include "BlocksGrid.h"
#include "GameObjects.h"
#include "CommonFuncs.h"

BlocksGrid::BlocksGrid(const QRectF& box, const int dimensions[2]) {
  _lowLeftCorner = box.topLeft();
  QPointF max = box.bottomRight();

  _dims[0] = dimensions[0];
  _dims[1] = dimensions[1];

  _dimSteps[0] = (max.x() - _lowLeftCorner.x()) / _dims[0];
  _dimSteps[1] = (max.y() - _lowLeftCorner.y()) / _dims[1];

  _unbreakBlocksNumber = 0;
}

QRectF BlocksGrid::calcBox(size_t row, size_t column) const {
  QPointF min(_lowLeftCorner.x() + column * _dimSteps[0], _lowLeftCorner.y() + row * _dimSteps[1]);
  QPointF max = min + QPointF(_dimSteps[0], _dimSteps[1]);
  return QRectF(min, max);
}

Block* BlocksGrid::CreateBlock(size_t i, size_t j, int health, BlockType type) {
  QRectF box = calcBox(i, j);
  Block* newBlock = new Block(box, health, type);

  if (type == BlockType::UNBREAKABLE) {
    _unbreakBlocksNumber++;
  }

  newBlock->gridPtr = this;
  newBlock->indexInGrid = i * _dims[0] + j;

  return newBlock;
}

void BlocksGrid::breakBlock(Block* block) {
  if (_movingBlocks.contains((MovingBlock*)block)) {
    _movingBlocks.removeOne((MovingBlock*)block);

  } else {
    _brokenBlocks.insert(block->index());
  }

  QGraphicsScene* scene = block->scene();
  scene->removeItem(block);
  delete block;
}

QRectF BlocksGrid::emptyBox(const QList<Ball*>& balls, int& boxIndex) const {
  QRectF emptyBox;

  for (int index : _brokenBlocks) {
    int row = index / _dims[0];
    int column = index % _dims[0];
    bool isEmpty = true;

    for (MovingBlock* mBlock : _movingBlocks) {
      if (row == mBlock->index() / _dims[0]) {
        isEmpty = false;
        break;
      }
    }

    if (isEmpty) {
      QRectF box = calcBox(row, column);

      for (Ball* ball : balls) {
        QPainterPath path = ball->shape().translated(ball->center());

        if (path.intersects(box)) {
          isEmpty = false;
          break;
        }
      }

      if (isEmpty) {
        emptyBox = box;
        boxIndex = index;
        break;
      }
    }
  }

  return emptyBox;
}

MovingBlock* BlocksGrid::CreateMovingBlock(const QList<Ball*>& balls) {
  int boxIndex;
  QRectF box = emptyBox(balls, boxIndex);

  if (!box.isNull()) {
    MovingBlock* mBlock = new MovingBlock(box, 3, BlockType::DEFAULT, 1);
    mBlock->indexInGrid = boxIndex;
    mBlock->gridPtr = this;
    _movingBlocks.append(mBlock);
    return mBlock;
  }

  return nullptr;
}

void BlocksGrid::advance() {
  for (MovingBlock* mBlock : _movingBlocks) {

    if (mBlock->direct() == MovingBlock::Direction::LEFT) {
      double extremePoint = mBlock->left() - mBlock->speed();
      QRectF box = calcBox(mBlock->index() / _dims[0], mBlock->index() % _dims[0]);

      if (extremePoint < box.left()) {

        if (extremePoint < left()) {
          mBlock->moveLeft(left());
          mBlock->changeDirect();

        } else if (_brokenBlocks.find(mBlock->index() - 1) == _brokenBlocks.end()) {
          mBlock->moveLeft(box.left());
          mBlock->changeDirect();
        
        } else {
          mBlock->moveLeft(extremePoint);
          mBlock->changeIndex(mBlock->index() - 1);
        }
      
      } else {
        mBlock->moveLeft(extremePoint);
      }

    } else {
      double extremePoint = mBlock->right() + mBlock->speed();
      QRectF box = calcBox(mBlock->index() / _dims[0], mBlock->index() % _dims[0]);
      
      if (extremePoint > box.right()) {

        if (extremePoint > right()) {
          mBlock->moveRight(right());
          mBlock->changeDirect();

        } else if (_brokenBlocks.find(mBlock->index() + 1) == _brokenBlocks.end()) {
          mBlock->moveRight(box.right());
          mBlock->changeDirect();

        }
        else {
          mBlock->moveRight(extremePoint);
          mBlock->changeIndex(mBlock->index() + 1);
        }

      } else {
        mBlock->moveRight(extremePoint);
      }
    }

    mBlock->setPos(flipY(mBlock->center()));
  }
}

bool BlocksGrid::isEmpty() {
  return _brokenBlocks.size() == _dims[0] * _dims[1] - 
    _unbreakBlocksNumber && _movingBlocks.isEmpty();
}

QPointF BlocksGrid::center() const {
  return QPointF((left() + right()) / 2, (bottom() + top()) / 2);
}
