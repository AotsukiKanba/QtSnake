#ifndef SNAKECORE_H
#define SNAKECORE_H

#include <QtCore/QPoint>
#include <QtCore/QTime>
#include <QtGui/QWidget>

#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

#define MAP_WIDTH		45
#define MAP_HEIGHT		45

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
	enum GameObject { Empty, Food, Wall, Corpse, Item, SnakeP1, SnakeP2, SnakeP3, SnakeP4 };
	enum GameItem { None, Longer, SpeedUp, SpeedDown, Meteorite };
	enum GameDirection { Up, Left, Down, Right };

	CSnakeCore(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CSnakeCore();
	static int speedBySize(int size);

private Q_SLOTS:
	void onSnakeMove(int id, QPoint to, QPoint tail);
	inline void setBlockState(int x, int y, CSnakeCore::GameObject g) { m_map[x][y] = g; }

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
	void gameOver();
	void addFood();
	void addMeteorite();

	QPoint m_centerpt;
	QRect m_mainRect;

private:
	GameState m_state;
	GamePlayer m_player;
	GameItem m_item;
	GameObject m_map[MAP_WIDTH][MAP_HEIGHT];
	CSnake *m_snakes;
	int m_winner;
	QPoint m_foodpos;
	QPoint m_itempos;
	QTime m_starttime;
};

#endif // SNAKECORE_H
