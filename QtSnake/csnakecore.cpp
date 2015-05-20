#include "csnakecore.h"
#include "csnake.h"
#include <time.h>
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
		m_snakes[i].setId(i);
		connect(&m_snakes[i], SIGNAL(snakeMove(int, QPoint, QPoint)), this, SLOT(onSnakeMove(int, QPoint, QPoint)));
		connect(&m_snakes[i], SIGNAL(setBlock(int, int, CSnakeCore::GameObject)), this, SLOT(setBlockState(int, int, CSnakeCore::GameObject)));
	}

	qsrand(time(NULL));
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
	static QString t1 = QString::fromLocal8Bit("单人游戏");
	static QString t2 = QString::fromLocal8Bit("双人游戏");
	static QString t3 = QString::fromLocal8Bit("三人游戏");
	static QString t4 = QString::fromLocal8Bit("四人游戏");
	static QString title = "Snake";
	static QFont titleItemFont, titleFont;

	titleFont.setPointSize(90);
	titleFont.setBold(true);
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

	QFontMetrics tme(titleFont);
	p.setFont(titleFont);
	p.drawText(m_centerpt.x() - tme.width(title) / 2, 50 + tme.height(), title);
}

void CSnakeCore::drawGameObject(QPainter &p)
{
	QFont font = p.font();
	font.setBold(true);
	p.setPen(Qt::black);
	QColor cuc = Qt::white;
	int itemId = -1;
	for (int i = 0; i < MAP_HEIGHT; ++i)
		for (int k = 0; k < MAP_WIDTH; ++k)
		{
			if (m_map[i][k] == Empty)
				continue;

			switch (m_map[i][k])
			{
			case Snake:
				cuc = Qt::green;
				break;
				
			case Food:
				cuc = Qt::red;
				break;

			case Wall:
				cuc = Qt::white;
				break;

			case Item:
				cuc = Qt::blue;
				break;

			case Corpse:
				cuc = Qt::gray;
				break;
			}
			
			p.setBrush(cuc);
			QRect rect(25 + (k - 1) * 10, 25 + (i - 1) * 10, 10, 10);
			p.drawRect(rect);
			
			static QString s[4] = { "∏", "≡", "≈", "▲" };
			if (m_map[i][k] == Item && m_item != None)
			{
				p.setPen(Qt::white);
				p.setFont(font);
				p.drawText(rect.x() + 1, rect.y() + 1, rect.width(), rect.height(), Qt::AlignCenter, s[m_item - 1]);
				p.setPen(Qt::black);
			}
		}
}

void CSnakeCore::drawGamePause(QPainter &p)
{

}

void CSnakeCore::drawGameOver(QPainter &p)
{
	p.setBrush(QColor(0, 0, 0, 168));
	p.drawRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	static QString gotext = "Game Over";
	static QFont font;
	font.setPointSize(60);
	font.setBold(true);
	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(5);
	p.setPen(pen);
	p.setFont(font);
	QFontMetrics metrics(p.font());
	int w = metrics.width(gotext);
	int h = metrics.height();
	p.drawText(m_centerpt.x() - w / 2, m_centerpt.y() - h / 4, gotext);

	static QString keytip = QString::fromLocal8Bit("Esc键回到标题 回车重新开始");
	font.setPointSize(20);
	p.setFont(font);
	metrics = QFontMetrics(p.font());
	w = metrics.width(keytip);
	h = metrics.height();
	p.drawText(m_centerpt.x() - w / 2, m_centerpt.y() + h + 10, keytip);
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
		m_snakes[0].onChangeDirect(Down);
		break;

	case Qt::Key_Left:
		m_snakes[0].onChangeDirect(Left);
		break;

	case Qt::Key_Right:
		m_snakes[0].onChangeDirect(Right);
		break;

		//P2
	case Qt::Key_W:
		m_snakes[1].onChangeDirect(Up);
		break;

	case Qt::Key_S:
		m_snakes[1].onChangeDirect(Down);
		break;

	case Qt::Key_A:
		m_snakes[1].onChangeDirect(Left);
		break;

	case Qt::Key_D:
		m_snakes[1].onChangeDirect(Right);
		break;

		//P3
	case Qt::Key_I:
		m_snakes[2].onChangeDirect(Up);
		break;

	case Qt::Key_K:
		m_snakes[2].onChangeDirect(Down);
		break;

	case Qt::Key_J:
		m_snakes[2].onChangeDirect(Left);
		break;

	case Qt::Key_L:
		m_snakes[2].onChangeDirect(Right);
		break;

		//P4
	case Qt::Key_8:
		m_snakes[3].onChangeDirect(Up);
		break;

	case Qt::Key_5:
		m_snakes[3].onChangeDirect(Down);
		break;

	case Qt::Key_4:
		m_snakes[3].onChangeDirect(Left);
		break;

	case Qt::Key_6:
		m_snakes[3].onChangeDirect(Right);
		break;
	}
}

void CSnakeCore::keyGamePause(int key)
{

}

void CSnakeCore::keyGameOver(int key)
{
	if (key == Qt::Key_Enter || key == Qt::Key_Return)
		startGame();
	if (key == Qt::Key_Escape)
		m_state = GameTitle;
}

void CSnakeCore::startGame()
{
	m_state = GameStarting;
	m_item = None;
	m_winner = -1;
	m_foodpos = QPoint(0, 0);
	m_itempos = QPoint(0, 0);
	
	for (int i = 0; i < MAP_WIDTH; ++i)
		for (int k = 0; k < MAP_HEIGHT; ++k)
		{
			if (i == 0 || i == MAP_WIDTH - 1 || k == 0 || k == MAP_HEIGHT - 1)
				setBlockState(i, k, Wall);
			else
				setBlockState(i, k, Empty);
		}

	for (int i = 0; i <= m_player; ++i)
	{
		m_snakes[i].initSnake(i + 1);
		m_snakes[i].goSnake();
	}

	addFood();

	update();
}

int CSnakeCore::speedBySize(int size)
{
	int s = 300 - size * 10;
	return s > 50 ? s : 50;
}

void CSnakeCore::onSnakeMove(int id, QPoint to, QPoint tail)
{
	bool isgrow = false;

	GameObject g = m_map[to.y()][to.x()];
	if (g == Food)
		isgrow = true;
	if (g == Snake || g == Wall || g == Corpse)
		m_snakes[id].die();
	if (g == Item)
	{
		if (m_item == Meteorite)
			addMeteorite();
		else
			m_snakes[id].getItem(m_item);
	}

	if (m_snakes[id].isLiving())
	{
		setBlockState(tail.y(), tail.x(), Empty);
		setBlockState(to.y(), to.x(), Snake);
		m_snakes[id].move(to);

		if (isgrow)
		{
			m_snakes[id].grow(tail);
			addFood();
		}
	}
	else
	{
		if (m_player > P1)
		{
			int winner = -1;
			for (int i = 0; i < MAX_PLAYER; ++i)
			{
				if (m_snakes[i].isLiving())
				{
					if (winner == -1)
						winner = i;
					else
					{
						winner = -1;
						break;
					}
				}
			}
			if (winner != -1)
			{
				m_winner = winner;
				gameOver();
			}
		}
		else
		{
			gameOver();
		}
	}

	if (m_map[m_foodpos.x()][m_foodpos.y()] != Food)
		addFood();

	update();
}

void CSnakeCore::addFood()
{
	int x, y;
	for(;;)
	{
		x = qrand() % MAP_WIDTH;
		y = qrand() & MAP_HEIGHT;
		if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0)
			continue;

		if (m_map[x][y] == Empty)
		{
			setBlockState(x, y, Food);
			m_foodpos = QPoint(x, y);
			break;
		}
	}

	if (m_map[m_itempos.x()][m_itempos.y()] != Item)
		m_item = None;
	if (m_item != None)
		return ;

	int i = qrand() % 4;
	if (i == 0)
	{
		for(;;)
		{
			x = qrand() % MAP_WIDTH;
			y = qrand() & MAP_HEIGHT;
			if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0)
				continue;

			if (m_map[x][y] == Empty)
			{
				m_item = GameItem(qrand() % 5);
				if (m_item <= 0 || m_item >=5)
					continue;
				setBlockState(x, y, Item);
				m_itempos = QPoint(x, y);
				break;
			}
		}
	}
}

void CSnakeCore::gameOver()
{
	m_state = GameOver;
	for (int i = 0; i < MAX_PLAYER; ++i)
		m_snakes[i].stopSnake();
}

void CSnakeCore::addMeteorite()
{
	int x, y;
	for(;;)
	{
		x = qrand() % MAP_WIDTH;
		y = qrand() & MAP_HEIGHT;
		if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0)
			continue;
		if (m_map[x][y] == Empty)
		{
			setBlockState(x, y, Wall);
			break;
		}
	}
}
