#ifndef SNAKECORE_H
#define SNAKECORE_H

#include <QtCore/QPoint>
#include <QtGui/QWidget>

#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

#define MAP_WIDTH		50
#define MAP_HEIGHT		50

#define MAX_PLAYER		4

class QPainter;
class QKeyEvent;
class CSnake;

class CSnakeCore : public QWidget
{
	Q_OBJECT;

public:
	enum GameState { GameTitle, GameStarting, GamePause, GameOver };
	enum GamePlayer { P1, P2, P3, P4 };
	enum GameObject { Empty, Snake, Food, Wall, Corpse, Item };
	enum GameItem { None };
	enum GameDirection { Up, Dwon, Left, Right };

	CSnakeCore(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CSnakeCore();
	static int speedBySize(int size);

private Q_SLOTS:
	void onSnakeMove(QPoint to, QPoint tail);

protected:
	void paintEvent(QPaintEvent *e);
	void keyPressEvent(QKeyEvent *e);

	void drawGameTitle(QPainter &p);
	void drawGameObject(QPainter &p);
	void drawGamePause(QPainter &p);
	void drawGameOver(QPainter &p);

	void keyGameTitle(int key);
	void keyGameStarting(int key);
	void keyGamePause(int key);
	void keyGameOver(int key);

	void startGame();

	QPoint m_centerpt;
	QRect m_mainRect;

private:
	GameState m_state;
	GamePlayer m_player;
	GameObject m_map[MAP_WIDTH][MAP_HEIGHT];
	CSnake *m_snakes;
};

#endif // SNAKECORE_H
