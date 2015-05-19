#include "csnakecore.h"
#include "csnake.h"
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPalette>

CSnakeCore::CSnakeCore(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
	, m_state(GameTitle)
	, m_player(P1)
{
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);  
	setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);  
	setWindowTitle("QtSnake");

	QPalette palette(this->palette());
	palette.setBrush(QPalette::Background, Qt::black);
	setPalette(palette);

	m_centerpt.setX(WINDOW_WIDTH / 2);
	m_centerpt.setY(WINDOW_HEIGHT / 2);

	m_snakes = new CSnake[MAX_PLAYER];
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		connect(&m_snakes[i], SIGNAL(snakeMove(QPoint, QPoint)), this, SLOT(onSnakeMove(QPoint, QPoint)));
	}
}

CSnakeCore::~CSnakeCore()
{

}

void CSnakeCore::paintEvent(QPaintEvent *e)
{
	QPainter p(this);

	switch (m_state)
	{
	case GameTitle:
		drawGameTitle(p);
		break;

	case GameStarting:
		drawGameObject(p);
		break;

	case GamePause:
		drawGameObject(p);
		drawGamePause(p);
		break;

	case GameOver:
		drawGameObject(p);
		drawGameOver(p);
		break;
	}
}

void CSnakeCore::keyPressEvent(QKeyEvent *e)
{
	int key = e->key();
	switch (m_state)
	{
	case GameTitle:
		keyGameTitle(key);
		break;

	case GameStarting:
		keyGameStarting(key);
		break;

	case GamePause:
		keyGamePause(key);
		break;

	case GameOver:
		keyGameOver(key);
		break;
	}

	update();
}

void CSnakeCore::drawGameTitle(QPainter &p)
{
	static QString t1 = "单人游戏";
	static QString t2 = "双人游戏";
	static QString t3 = "三人游戏";
	static QString t4 = "四人游戏";
	static QFont titleItemFont;

	titleItemFont.setPointSize(16);
	titleItemFont.setBold(true);
	p.setFont(titleItemFont);

	QFontMetrics metrics(p.font());
	p.setPen(Qt::white);
	int x = m_centerpt.x() - metrics.width(t1) / 2;
	int spc = 20;
	int y = m_centerpt.y();
	int h = metrics.height();
	int w = metrics.width(t1);
	int ty[4] = { y + h, y + 2 * h + spc, y + 3 * h + 2 * spc, y + 4 * h + 3 * spc };
	p.drawText(x, ty[0], t1);
	p.drawText(x, ty[1], t2);
	p.drawText(x, ty[2], t3);
	p.drawText(x, ty[3], t4);

	int pn = m_player;
	Q_ASSERT(pn >= 0 && pn <= 3);

	int rectSpc = 10;
	int rectSpc2 = 2 * rectSpc;
	QPen pen;
	pen.setWidth(5);
	pen.setBrush(Qt::white);
	p.setPen(pen);
	p.drawRect(x - rectSpc, ty[pn] - rectSpc - h + 3, w + rectSpc2, h + rectSpc2);
}

void CSnakeCore::drawGameObject(QPainter &p)
{

}

void CSnakeCore::drawGamePause(QPainter &p)
{

}

void CSnakeCore::drawGameOver(QPainter &p)
{

}

void CSnakeCore::keyGameTitle(int key)
{
	switch (key)
	{
	case Qt::Key_Up:
		m_player = GamePlayer((m_player + 4 - 1) % 4);
		break;

	case Qt::Key_Down:
		m_player = GamePlayer((m_player + 1) % 4);
		break;

	case Qt::Key_Left:
		break;

	case Qt::Key_Right:
		break;

	case Qt::Key_Enter:
	case Qt::Key_Return:
		startGame();
		break;
	}
}

void CSnakeCore::keyGameStarting(int key)
{
	switch (key)
	{
		//P1
	case Qt::Key_Up:
		m_snakes[0].onChangeDirect(Up);
		break;

	case Qt::Key_Down:
		m_snakes[0].onChangeDirect(Dwon);
		break;

	case Qt::Key_Left:
		m_snakes[0].onChangeDirect(Left);
		break;

	case Qt::Key_Right:
		m_snakes[0].onChangeDirect(Right);
		break;

		//P2
	case Qt::Key_W:
		break;

	case Qt::Key_S:
		break;

	case Qt::Key_A:
		break;

	case Qt::Key_D:
		break;
	}
}

void CSnakeCore::keyGamePause(int key)
{

}

void CSnakeCore::keyGameOver(int key)
{

}

void CSnakeCore::startGame()
{
	m_state = GameStarting;
	
	for (int i = 0; i < MAP_WIDTH; ++i)
	{
		for (int k = 0; k < MAP_HEIGHT; ++k)
		{
			if (i == 0 || i == MAP_WIDTH - 1 || k == 0 || k == MAP_HEIGHT - 1)
				m_map[i][k] = Wall;
		}
	}

	for (int i = 0; i <= m_player; ++i)
	{
		m_snakes[i].initSnake(i + 1);
		m_snakes[i].goSnake();
	}

	update();
}

int CSnakeCore::speedBySize(int size)
{
	int s = 500 - size * 20;
	return s > 0 ? s : 0;
}

void CSnakeCore::onSnakeMove(QPoint to, QPoint tail)
{
	m_map[tail.x()][tail.y()] = Empty;
	m_map[to.x()][to.y()] = Snake;
}
