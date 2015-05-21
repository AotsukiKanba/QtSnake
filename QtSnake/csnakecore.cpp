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
	static QColor gocolors[] = { Qt::transparent, Qt::red, Qt::white, Qt::gray, Qt::black, Qt::green, Qt::blue, Qt::cyan, Qt::magenta };
	static QString ss[5] = { "", "♂", "≡", "≈", "▲" };
	static QString sd[5] = { "", QString::fromLocal8Bit("增长"),  QString::fromLocal8Bit("加速"),  QString::fromLocal8Bit("减速"),  QString::fromLocal8Bit("陨石") };

	QFont font = p.font();
	font.setBold(true);
	p.setPen(Qt::black);
	QColor cuc = Qt::white;
	int itemId = -1;
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{	for (int k = 0; k < MAP_WIDTH; ++k)
		{
			if (m_map[i][k] == Empty)
				continue;
			cuc = gocolors[m_map[i][k]];

			p.setBrush(cuc);
			QRect rect(25 + (k - 1) * 10, 25 + (i - 1) * 10, 10, 10);
			p.drawRect(rect);
			
			if (m_map[i][k] == Item && m_item != None)
			{
				p.setPen(Qt::white);
				p.setFont(font);
				p.drawText(rect.x() + 2, rect.y() + 2, rect.width(), rect.height(), Qt::AlignCenter, ss[m_item]);
				p.setPen(Qt::black);
			}
		}
	}

	// GameInfo
	int ix = 25 + MAP_WIDTH * 10 + 10;
	int iy = 35;
	int icx = WINDOW_WIDTH - 30;
	int ispc = 70;
	QFont capfont;
	capfont.setPointSize(16);
	capfont.setFamily(QString::fromLocal8Bit("微软雅黑"));
	capfont.setBold(true);
	QFont texfont = capfont;
	texfont.setPointSize(10);
	QFontMetrics capmt(capfont);
	QFontMetrics texmt(texfont);
	int texspc = 1;

	QString ctrls[4] = {"↑ ↓ ← →", "W S A D", "I K J L", "8 5 4 6"};
	for (int i = 0; i <= m_player; ++i)
	{
		p.setFont(capfont);
		p.setPen(gocolors[SnakeP1 + i]);
		int ty = iy + i * ispc;
		p.drawText(ix, ty, QString::number(i + 1) + "P");
		p.setFont(texfont);
		p.setPen(Qt::white);
		ty += capmt.height() * 0.7 + texspc;
		p.drawText(ix, ty, QString::fromLocal8Bit("得分: ") + QString::number((m_snakes[i].size() - 3) * 100));
		ty += texmt.height() + texspc;
		p.drawText(ix, ty, QString::fromLocal8Bit("控制键: ") + ctrls[i]);
	}

	int ity = iy + MAX_PLAYER * ispc;
	p.setFont(capfont);
	p.setPen(QColor(250, 192, 128));
	p.drawText(ix, ity, QString::fromLocal8Bit("道具列表"));
	p.setPen(Qt::white);
	p.setFont(texfont);
	for (int i = 0; i < 4; ++i)
	{
		int itdy = ity + capmt.height() * 0.8 + i * (texmt.height() + texspc);
		p.drawText(ix, itdy, ss[i + 1]);
		p.drawText(ix + 15, itdy, ": " + sd[i + 1]);
	}

	int sty = ity + capmt.height() + 4 * (texmt.height() + texspc) + 5;
	p.setFont(capfont);
	p.setPen(QColor(255, 97, 0));
	p.drawText(ix, sty, QString::fromLocal8Bit("游戏时间"));
	p.setPen(Qt::white);
	int time = m_starttime.secsTo(QTime::currentTime());
	int mn = time / 60;
	int se = time % 60;
	QString smn = (mn < 10) ? QString("0%1").arg(mn) : QString::number(mn);
	QString sse = (se < 10) ? QString("0%1").arg(se) : QString::number(se);
	p.drawText(ix, sty + capmt.height() + texspc, QString("%1 : %2").arg(smn).arg(sse));
}

void CSnakeCore::drawGamePause(QPainter &p)
{

}

void CSnakeCore::drawGameOver(QPainter &p)
{
	p.setBrush(QColor(0, 0, 0, 168));
	p.drawRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	QString s = m_player == P1 ? "Game Over" : "Winner P" + QString::number(m_winner + 1);
	QFont font;
	font.setPointSize(60);
	font.setBold(true);
	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(5);
	p.setPen(pen);
	p.setFont(font);
	QFontMetrics metrics(p.font());
	int w = metrics.width(s);
	int h = metrics.height();
	p.drawText(m_centerpt.x() - w / 2, m_centerpt.y() - h / 4, s);

	static QString keytip = QString::fromLocal8Bit("Esc键返回菜单 回车重新开始");
	font.setPointSize(18);
	p.setFont(font);
	metrics = QFontMetrics(p.font());
	w = metrics.width(keytip);
	h = metrics.height();
	p.drawText(m_centerpt.x() - w / 2, m_centerpt.y() + h + 40, keytip);
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
	m_starttime = QTime::currentTime();
	
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
	int s = 300 - size * 5;
	return s > 40 ? s : 40;
}

void CSnakeCore::onSnakeMove(int id, QPoint to, QPoint tail)
{
	bool isgrow = false;

	GameObject g = m_map[to.y()][to.x()];
	if (g == Food)
		isgrow = true;
	if (g == SnakeP1 || g == SnakeP2 || g == SnakeP3 || g == SnakeP4 || g == Wall || g == Corpse)
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
		setBlockState(to.y(), to.x(), GameObject(SnakeP1 + id));
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

	int i = qrand() % 22;
	if (i % 3 == 0)
	{
		if (m_map[m_itempos.x()][m_itempos.y()] == Item)
			setBlockState(m_itempos.x(), m_itempos.y(), Empty);
		for(;;)
		{
			x = qrand() % MAP_WIDTH;
			y = qrand() & MAP_HEIGHT;
			if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0)
				continue;

			if (m_map[x][y] == Empty)
			{
				m_item = GameItem(qrand() % 5);
				if (m_item <= 0 || m_item >= 5)
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
