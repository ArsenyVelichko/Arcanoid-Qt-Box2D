#ifndef BLOCKSGRIG_H
#define BLOCKSGRIG_H

#include <QList>
#include <QRect>
#include <QVector2D>
#include <unordered_set>
#include <QGraphicsScene>

class Block;
class MovingBlock;
enum class BlockType;
class Ball;

class BlocksGrid {

  int _dims[2];
  int _unbreakBlocksNumber;
  QPointF _lowLeftCorner;
  QVector2D _dimSteps;
  std::unordered_set<int> _brokenBlocks;
  QList<MovingBlock*> _movingBlocks;

  QRectF calcBox(size_t row, size_t column) const;
  QRectF emptyBox(const QList<Ball*>& balls, int& boxIndex) const;

public:
  BlocksGrid(const QRectF& box, const int dimensions[2]);
  
  Block* CreateBlock(size_t i, size_t j, int health, BlockType type);
  MovingBlock* CreateMovingBlock(const QList<Ball*>& balls);
  void breakBlock(Block* block);
  void addEmptyBlock(size_t i, size_t j) { _brokenBlocks.insert(i * _dims[0] + j); }

  double yStep() const { return _dimSteps[1]; }
  double xStep() const { return _dimSteps[0]; }

  double left() const { return _lowLeftCorner.x(); }
  double right() const { return _lowLeftCorner.x() + _dims[0] * _dimSteps[0]; }
  double bottom() const { return _lowLeftCorner.y(); }
  double top() const { return _lowLeftCorner.y() + _dims[1] * _dimSteps[1]; }

  QPointF center() const;

  void advance();
  bool isEmpty();
};

#endif //BLOCKSGRIG_H